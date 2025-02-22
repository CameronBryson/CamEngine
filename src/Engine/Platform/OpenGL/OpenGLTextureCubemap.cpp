#include "pch.hpp" 
#include "OpenGLTextureCubemap.hpp"

#include <stb_image.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "Engine/Graphics/Abstract/Shader.hpp"
#include "OpenGLUtil.hpp"  

// Include the abstract FrameBuffer interface
#include <FrameBuffer.hpp>

// Resolution (width and height) of each cubemap face.
static const unsigned int ENV_MAP_SIZE = 512;

// Projection matrix for 90° FOV.
static glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

// View matrices for each cubemap face.
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

OpenGLTextureCubemap::OpenGLTextureCubemap(const std::string& hdrPath,
    const std::shared_ptr<Shader>& equirectShader)
{
    stbi_set_flip_vertically_on_load(true);
    int w, h, nrComponents;
    float* hdrData = stbi_loadf(hdrPath.c_str(), &w, &h, &nrComponents, 0);
    if (!hdrData)
    {
        std::cerr << "Failed to load HDR: " << hdrPath << std::endl;
        return;
    }

    GLuint hdrTexID;
    GL_CHECK(glGenTextures(1, &hdrTexID));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, hdrTexID));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, hdrData));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    stbi_image_free(hdrData);

    GL_CHECK(glGenTextures(1, &textureID));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, textureID));
    for (unsigned int i = 0; i < 6; ++i)
    {
        GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            ENV_MAP_SIZE, ENV_MAP_SIZE, 0, GL_RGB, GL_FLOAT, nullptr));
    }
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    width = ENV_MAP_SIZE;
    height = ENV_MAP_SIZE;

    auto fbo = FrameBuffer::createFrameBuffer(
        ENV_MAP_SIZE, ENV_MAP_SIZE,
        { { FrameBufferAttachmentType::Depth, FrameBufferTextureFormat::Depth24 } }
    );

    // 4) Configure the shader and bind the HDR texture.
    equirectShader->use();
    equirectShader->setMat4("projection", captureProjection);
    equirectShader->setInt("equirectangularMap", 0);
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, hdrTexID));

    GL_CHECK(glViewport(0, 0, ENV_MAP_SIZE, ENV_MAP_SIZE));
    fbo->bind();
    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectShader->setMat4("view", captureViews[i]);
        fbo->attachExternalTexture(
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            textureID,
            0
        );
		fbo->setDrawBuffers({ GL_COLOR_ATTACHMENT0 });
        fbo->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        OpenGlUtil::drawCube();
    }
    fbo->unbind();

    GL_CHECK(glDeleteTextures(1, &hdrTexID));
}

OpenGLTextureCubemap::OpenGLTextureCubemap(GLuint id, int w, int h)
    : textureID(id), width(w), height(h)
{
}

OpenGLTextureCubemap::~OpenGLTextureCubemap()
{
    GL_CHECK(glDeleteTextures(1, &textureID));
}

void OpenGLTextureCubemap::bind(unsigned int slot)
{
	unbind(slot);
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, textureID));
}

void OpenGLTextureCubemap::unbind(unsigned int slot)
{
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
}

int OpenGLTextureCubemap::getWidth() const { return width; }
int OpenGLTextureCubemap::getHeight() const { return height; }

void OpenGLTextureCubemap::setShadowSamplerParameters()
{
    bind(0);  // This could be the issue - binding to unit 0
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    unbind(0);
    isShadowSampler = true;
}

void OpenGLTextureCubemap::setNormalSamplerParameters()
{
    bind(0);
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    unbind(0);
    isShadowSampler = true;
}
