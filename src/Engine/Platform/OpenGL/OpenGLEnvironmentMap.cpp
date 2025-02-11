#include "pch.hpp"
#include "OpenGLEnvironmentMap.hpp"

#include "Engine/Platform/OpenGL/OpenGLTextureCubemap.hpp"
#include "Engine/Platform/OpenGL/OpenGLTexture2D.hpp"
#include "Engine/Graphics/Abstract/TextureCubemap.hpp"
#include "Engine/Graphics/Abstract/Texture2D.hpp"
#include "Engine/Graphics/Abstract/Shader.hpp"
#include "Engine/Graphics/TextureSlots.hpp"
#include "Engine/Util/OpenGLUtil.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
#include <stb_image.h>

// -----------------------------------------------------------------------------
// These matrices are used when rendering to cubemap faces.
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
    // Save the current viewport to restore it later.
    GLint oldViewport[4];
    glGetIntegerv(GL_VIEWPORT, oldViewport);
	mSkyboxCubemap = TextureCubemap::createTextureCubemap(hdrPath, equirectangularToCubemapShader);


    generateIrradianceMap();
    generatePrefilterMap();
    generateBRDFLUT();
    glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);
}

void OpenGLEnvironmentMap::generateIrradianceMap()
{
    const unsigned int irradianceSize = 32;
    GLuint irradianceCubemapID;
    glGenTextures(1, &irradianceCubemapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceCubemapID);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            irradianceSize, irradianceSize, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    mIrradianceCubemap = TextureCubemap::createTextureCubemap(irradianceCubemapID, irradianceSize, irradianceSize);

    GLuint captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, irradianceSize, irradianceSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    mIrradianceShader->use();
    mIrradianceShader->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mSkyboxCubemap->getTextureID());

    glViewport(0, 0, irradianceSize, irradianceSize);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        mIrradianceShader->setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceCubemapID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        OpenGlUtil::drawCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);
}

void OpenGLEnvironmentMap::generatePrefilterMap()
{
    const unsigned int prefilterSize = 128;
    GLuint prefilterCubemapID;
    glGenTextures(1, &prefilterCubemapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterCubemapID);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            prefilterSize, prefilterSize, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    mPrefilterCubemap = TextureCubemap::createTextureCubemap(prefilterCubemapID, prefilterSize, prefilterSize);

    GLuint captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    mPrefilterShader->use();
    mPrefilterShader->setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mSkyboxCubemap->getTextureID());

    const unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        unsigned int mipWidth = prefilterSize * std::pow(0.5, mip);
        unsigned int mipHeight = mipWidth;
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (maxMipLevels - 1);
        mPrefilterShader->setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            mPrefilterShader->setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterCubemapID, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            OpenGlUtil::drawCube();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);
}

void OpenGLEnvironmentMap::generateBRDFLUT()
{
    const unsigned int brdfLUTSize = 512;
    GLuint brdfLUTID;
    glGenTextures(1, &brdfLUTID);
    glBindTexture(GL_TEXTURE_2D, brdfLUTID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, brdfLUTSize, brdfLUTSize, 0, GL_RG, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    mBRDFLUT = Texture2D::createTexture2D(brdfLUTID, brdfLUTSize, brdfLUTSize);

    GLuint captureFBO;
    glGenFramebuffers(1, &captureFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTID, 0);

    glViewport(0, 0, brdfLUTSize, brdfLUTSize);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mBRDFShader->use();
    OpenGlUtil::drawQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &captureFBO);
}

// -----------------------------------------------------------------------------
// Binding functions for use in your PBR shader pass.
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
// Draw the skybox using the environment cubemap.
// -----------------------------------------------------------------------------
void OpenGLEnvironmentMap::drawSkybox(std::shared_ptr<Shader>& skyboxShader)
{
    glDepthFunc(GL_LEQUAL);
    skyboxShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mSkyboxCubemap->getTextureID());
    OpenGlUtil::drawCube();
    glDepthFunc(GL_LESS);
}

