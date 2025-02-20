#include "pch.hpp"
#include "OpenGLEnvironmentMap.hpp"

#include "Engine/Platform/OpenGL/OpenGLTextureCubemap.hpp"
#include "Engine/Platform/OpenGL/OpenGLTexture2D.hpp"
#include "Engine/Graphics/Abstract/TextureCubemap.hpp"
#include "Engine/Graphics/Abstract/Texture2D.hpp"
#include "Engine/Graphics/Abstract/Shader.hpp"
#include "Engine/Graphics/TextureSlots.hpp"
#include "Engine/Util/OpenGLUtil.hpp"

#include <FrameBuffer.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
#include <stb_image.h>


static glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 10.f);
static glm::mat4 captureViews[] =
{
    // +X face
    glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec3(0.0f, -1.0f,  0.0f)),
    // -X face
    glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f),  glm::vec3(0.0f, -1.0f,  0.0f)),
    // +Y face (Top)
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec3(0.0f,  0.0f,  1.0f)),
    // -Y face (Bottom)
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec3(0.0f,  0.0f, -1.0f)),
    // +Z face
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f),  glm::vec3(0.0f, -1.0f,  0.0f)),
    // -Z face
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f),  glm::vec3(0.0f, -1.0f,  0.0f))
};

// -----------------------------------------------------------------------------
OpenGLEnvironmentMap::OpenGLEnvironmentMap(const std::string& hdrPath,
    std::shared_ptr<Shader> equirectangularToCubemapShader,
    std::shared_ptr<Shader> irradianceShader,
    std::shared_ptr<Shader> prefilterShader,
    std::shared_ptr<Shader> brdfShader)
    : mEquirectangularToCubemapShader(equirectangularToCubemapShader)
    , mIrradianceShader(irradianceShader)
    , mPrefilterShader(prefilterShader)
    , mBRDFShader(brdfShader)
{
    // Save the current viewport
    GLint oldViewport[4];
    GL_CHECK(glGetIntegerv(GL_VIEWPORT, oldViewport));

    // Create the main skybox cubemap from the HDR path
    mSkyboxCubemap = TextureCubemap::createTextureCubemap(hdrPath, equirectangularToCubemapShader);

    // Now generate the environment-based maps
    generateIrradianceMap();
    generatePrefilterMap();
    generateBRDFLUT();

    // Restore old viewport
    GL_CHECK(glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]));
}

OpenGLEnvironmentMap::~OpenGLEnvironmentMap()
{
}


void OpenGLEnvironmentMap::generateIrradianceMap()
{
    const unsigned int irradianceSize = 32;

    GLuint irradianceCubemapID;
    GL_CHECK(glGenTextures(1, &irradianceCubemapID));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceCubemapID));
    for (unsigned int i = 0; i < 6; ++i)
    {
        GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            irradianceSize, irradianceSize, 0, GL_RGB, GL_FLOAT, nullptr));
    }
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    mIrradianceCubemap = TextureCubemap::createTextureCubemap(irradianceCubemapID, irradianceSize, irradianceSize);


    auto fbo = FrameBuffer::createFrameBuffer(
        irradianceSize,
        irradianceSize,
        {
            { FrameBufferAttachmentType::Depth, FrameBufferTextureFormat::Depth24 }
        }
    );
    fbo->bind();

    // Setup the shader
    mIrradianceShader->use();
    mIrradianceShader->setMat4("projection", captureProjection);
	mIrradianceShader->setInt("environmentMap", IBLSlots::IRRADIANCE);
    mSkyboxCubemap->bind(IBLSlots::IRRADIANCE);

    GL_CHECK(glViewport(0, 0, irradianceSize, irradianceSize));

    // For each face, attach that face as color 0 and draw
    for (unsigned int i = 0; i < 6; ++i)
    {
        mIrradianceShader->setMat4("view", captureViews[i]);

        fbo->attachExternalTexture(
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            irradianceCubemapID,
            0
        );
        fbo->setDrawBuffers({ GL_COLOR_ATTACHMENT0 });

        fbo->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        OpenGlUtil::drawCube();
    }

    fbo->unbind();
}

void OpenGLEnvironmentMap::generatePrefilterMap()
{
    const unsigned int prefilterSize = 128;

    GLuint prefilterCubemapID;
    GL_CHECK(glGenTextures(1, &prefilterCubemapID));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterCubemapID));
    for (unsigned int i = 0; i < 6; ++i)
    {
        GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            prefilterSize, prefilterSize, 0, GL_RGB, GL_FLOAT, nullptr));
    }
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));

    mPrefilterCubemap = TextureCubemap::createTextureCubemap(prefilterCubemapID, prefilterSize, prefilterSize);

    auto fbo = FrameBuffer::createFrameBuffer(
        prefilterSize,
        prefilterSize,
        {
            { FrameBufferAttachmentType::Depth, FrameBufferTextureFormat::Depth24 }
        }
    );

    mPrefilterShader->use();
    mPrefilterShader->setMat4("projection", captureProjection);
    mPrefilterShader->setInt("environmentMap", IBLSlots::PREFILTER);
    mSkyboxCubemap->bind(IBLSlots::PREFILTER);

    const unsigned int maxMipLevels = 5;

    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {

        unsigned int mipWidth = (unsigned int)(prefilterSize * std::pow(0.5, mip));
        unsigned int mipHeight = mipWidth;
        fbo->resize(mipWidth, mipHeight);

        GL_CHECK(glViewport(0, 0, mipWidth, mipHeight));
        fbo->bind();

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        mPrefilterShader->setFloat("roughness", roughness);

        // For each face, attach that face + mip, then clear & draw
        for (unsigned int i = 0; i < 6; ++i)
        {
            mPrefilterShader->setMat4("view", captureViews[i]);

            fbo->attachExternalTexture(
                GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                prefilterCubemapID,
                mip
            );
            fbo->setDrawBuffers({ GL_COLOR_ATTACHMENT0 });

            fbo->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            OpenGlUtil::drawCube();
        }

        fbo->unbind();
    }
}

void OpenGLEnvironmentMap::generateBRDFLUT()
{
    const unsigned int brdfLUTSize = 512;

    // Create the 2D LUT texture
    GLuint brdfLUTID;
    GL_CHECK(glGenTextures(1, &brdfLUTID));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, brdfLUTID));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F,
        brdfLUTSize, brdfLUTSize, 0,
        GL_RG, GL_FLOAT, nullptr));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    mBRDFLUT = Texture2D::createTexture2D(brdfLUTID, brdfLUTSize, brdfLUTSize);

    auto fbo = FrameBuffer::createFrameBuffer(
        brdfLUTSize,
        brdfLUTSize,
        { {FrameBufferAttachmentType::Color, FrameBufferTextureFormat::RG16F} }
    );
    fbo->bind();
    GL_CHECK(glViewport(0, 0, brdfLUTSize, brdfLUTSize));

    fbo->attachExternalTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTID, 0);
    fbo->setDrawBuffers({ GL_COLOR_ATTACHMENT0 });

    // Clear + draw
    fbo->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mBRDFShader->use();
    OpenGlUtil::drawQuad();

    fbo->unbind();
}

void OpenGLEnvironmentMap::bindIrradiance(int slot)
{
    mIrradianceCubemap->bind(slot);
}

void OpenGLEnvironmentMap::bindPrefilter(int slot)
{

    mPrefilterCubemap->bind(slot);
}

void OpenGLEnvironmentMap::bindBRDFLUT(int slot)
{
    mBRDFLUT->bind(slot);
}

void OpenGLEnvironmentMap::drawSkybox(std::shared_ptr<Shader>& skyboxShader)
{
    glDepthFunc(GL_LEQUAL);
    skyboxShader->use();
	skyboxShader->setInt("skybox", IBLSlots::SKYBOX);
    mSkyboxCubemap->bind(IBLSlots::SKYBOX);
    OpenGlUtil::drawCube();
    glDepthFunc(GL_LESS);
}
