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
    glGenTextures(1, &hdrTexID);
    glBindTexture(GL_TEXTURE_2D, hdrTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, hdrData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(hdrData);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
            ENV_MAP_SIZE, ENV_MAP_SIZE, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexID);

    glViewport(0, 0, ENV_MAP_SIZE, ENV_MAP_SIZE);
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

    glDeleteTextures(1, &hdrTexID);
}

OpenGLTextureCubemap::OpenGLTextureCubemap(GLuint id, int w, int h)
    : textureID(id), width(w), height(h)
{
}

OpenGLTextureCubemap::~OpenGLTextureCubemap()
{
    glDeleteTextures(1, &textureID);
}

void OpenGLTextureCubemap::bind(unsigned int slot)
{
	unbind(slot);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}

void OpenGLTextureCubemap::unbind(unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

int OpenGLTextureCubemap::getWidth() const { return width; }
int OpenGLTextureCubemap::getHeight() const { return height; }
