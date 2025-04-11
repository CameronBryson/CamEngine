#include "pch.hpp"
#include "Texture.hpp"

#include <stdexcept>
#include <iostream>
#include "Engine/Util/EngineUtil.hpp"
#include "stb_image.h"
#include <glad/glad.h>
#include "OpenGLUtil.hpp"
#include "Shader.hpp"
#include "FrameBuffer.hpp"
#include "Engine/Util/Logging.hpp"
#include "Engine/Util/ErrorHandler.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

// Constants for cubemap generation
static const unsigned int ENV_MAP_SIZE = 512;

// Projection matrix for 90° FOV (for cubemap generation)
static glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

// View matrices for each cubemap face
static glm::mat4 captureViews[] =
{
    // +X face
    glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    // -X face
    glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    // +Y face (Top)
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
    // -Y face (Bottom)
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
    // +Z face
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    // -Z face
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
};

// Create an empty 2D texture with specified dimensions and format
Texture::Texture(int width, int height, Format format)
    : mWidth(width), mHeight(height), mFormat(format), mType(Type::TEXTURE_2D)
{
    mTextureID = createEmptyTexture2D(width, height, format);
    
    // Set default parameters
    setFilterMode(FilterMode::Linear, FilterMode::Linear);
    setWrapMode(WrapMode::Repeat, WrapMode::Repeat);
}

// Load a 2D texture from a file
Texture::Texture(const std::string& filePath)
    : mTextureID(0), mWidth(0), mHeight(0), mType(Type::TEXTURE_2D)
{
    // Generate and bind the texture
    GL_CHECK(glGenTextures(1, &mTextureID));
    if (mTextureID == 0)
    {
        throw std::runtime_error("Failed to generate texture ID.");
    }
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureID));

    // Load the texture data using stb_image
    int nrChannels;
    stbi_set_flip_vertically_on_load(false); // Don't flip, maintain your current convention
    std::string fullPath = filePath;
    unsigned char* data = stbi_load(fullPath.c_str(), &mWidth, &mHeight, &nrChannels, 0);
    if (!data)
    {
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0)); // Unbind the texture
        GL_CHECK(glDeleteTextures(1, &mTextureID));
        throw std::runtime_error("Failed to load texture: " + filePath);
    }
    LOG_INFO(logging::gResourceLogger, "Loaded texture: {}", filePath);

    // Determine the texture format
    GLenum format;
    GLenum internalFormat;

    if (nrChannels == 1)
    {
        format = GL_RED;
        internalFormat = GL_RED;
        mFormat = Format::R;
    }
    else if (nrChannels == 2)
    {
        format = GL_RG;
        internalFormat = GL_RG8;
        mFormat = Format::RG;
    }
    else if (nrChannels == 3)
    {
        format = GL_RGB;
        internalFormat = GL_SRGB; // Assume diffuse textures are sRGB
        mFormat = Format::RGB;
    }
    else if (nrChannels == 4)
    {
        format = GL_RGBA;
        internalFormat = GL_SRGB_ALPHA; // Assume diffuse textures are sRGB
        mFormat = Format::RGBA;
    }
    else
    {
        LOG_WARN(logging::gGraphicsLogger, "Unknown format: {} channels. Defaulting to GL_RGB.", nrChannels);
        format = GL_RGB;
        internalFormat = GL_RGB8;
        mFormat = Format::RGB;
    }

    // Set texture wrapping based on format
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    // Upload the texture data to the GPU
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, mWidth, mHeight, 0, format, GL_UNSIGNED_BYTE, data));

    // Generate mipmaps for the texture
    GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));

    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0)); // Unbind the texture
        stbi_image_free(data);
        throw std::runtime_error("OpenGL error after uploading texture: " + std::to_string(error));
    }

    // Unbind the texture
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

    // Free the image data
    stbi_image_free(data);
}

// Create a cubemap texture from an HDR equirectangular map
Texture::Texture(const std::string& equirectangularMapPath, const std::shared_ptr<Shader>& equirectShader)
    : mTextureID(0), mWidth(0), mHeight(0), mType(Type::CUBEMAP), mFormat(Format::RGB16F)
{
    stbi_set_flip_vertically_on_load(true);
    int w, h, nrComponents;
    float* hdrData = stbi_loadf(equirectangularMapPath.c_str(), &w, &h, &nrComponents, 0);
    if (!hdrData)
    {
        LOG_ERROR(logging::gResourceLogger, "Failed to load HDR: {}", equirectangularMapPath);
        throw std::runtime_error("Failed to load HDR: " + equirectangularMapPath);
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

    GL_CHECK(glGenTextures(1, &mTextureID));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureID));
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

    mWidth = ENV_MAP_SIZE;
    mHeight = ENV_MAP_SIZE;
    auto fbo = std::make_shared<FrameBuffer>(
        ENV_MAP_SIZE, ENV_MAP_SIZE, std::vector<FrameBufferAttachmentSpecification>
    { { FrameBufferAttachmentType::Depth, FrameBufferTextureFormat::Depth24 } }
    );

    // Configure the shader and bind the HDR texture
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
            mTextureID,
            0
        );
        fbo->setDrawBuffers({ GL_COLOR_ATTACHMENT0 });
        fbo->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gl::drawCube();
    }
    fbo->unbind();

    GL_CHECK(glDeleteTextures(1, &hdrTexID));
}

// Create a texture from an Assimp texture object
Texture::Texture(const aiTexture* aiTex)
    : mTextureID(0), mWidth(0), mHeight(0), mType(Type::TEXTURE_2D)
{
    // Generate and bind the texture
    GL_CHECK(glGenTextures(1, &mTextureID));
    if (mTextureID == 0)
    {
        throw std::runtime_error("Failed to generate texture ID.");
    }
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, mTextureID));

    unsigned char* data = nullptr;
    GLenum format = GL_RGB;
    GLenum internalFormat = GL_RGB8;

    if (aiTex->mHeight == 0)
    {
        // Compressed texture (e.g., PNG, JPEG)
        int nrChannels;
        stbi_set_flip_vertically_on_load(false); // Don't flip, maintain your current convention
        data = stbi_load_from_memory(
            reinterpret_cast<const stbi_uc*>(aiTex->pcData),
            static_cast<int>(aiTex->mWidth),
            &mWidth,
            &mHeight,
            &nrChannels,
            0
        );

        if (data)
        {
            if (nrChannels == 1)
            {
                format = GL_RED;
                internalFormat = GL_RED;
                mFormat = Format::R;
            }
            else if (nrChannels == 2)
            {
                format = GL_RG;
                internalFormat = GL_RG8;
                mFormat = Format::RG;
            }
            else if (nrChannels == 3)
            {
                format = GL_RGB;
                internalFormat = GL_RGB8;
                mFormat = Format::RGB;
            }
            else if (nrChannels == 4)
            {
                format = GL_RGBA;
                internalFormat = GL_SRGB_ALPHA;
                mFormat = Format::RGBA;
            }
            else
            {
                format = GL_RGB; // Fallback
                internalFormat = GL_RGB8;
                mFormat = Format::RGB;
            }

            // Set texture wrapping based on format
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT));
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT));
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

            // Upload the texture data to the GPU
            GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, mWidth, mHeight, 0, format, GL_UNSIGNED_BYTE, data));

            // Generate mipmaps for the texture
            GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
        }
        else
        {
            LOG_ERROR(logging::gGraphicsLogger, "Failed to load embedded compressed texture.");
            GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0)); // Unbind the texture
            throw std::runtime_error("Failed to load embedded compressed texture.");
        }
    }
    else
    {
        // Uncompressed texture (RGBA8888 format)
        mWidth = static_cast<int>(aiTex->mWidth);
        mHeight = static_cast<int>(aiTex->mHeight);
        mFormat = Format::RGBA;

        // Allocate memory for texture data
        data = new unsigned char[mWidth * mHeight * 4]; // 4 channels (RGBA)

        // Copy data from aiTex->pcData (which is in aiTexel format)
        for (unsigned int y = 0; y < static_cast<unsigned int>(mHeight); ++y)
        {
            for (unsigned int x = 0; x < static_cast<unsigned int>(mWidth); ++x)
            {
                unsigned int index = y * mWidth + x;
                const aiTexel& texel = aiTex->pcData[index];
                data[index * 4 + 0] = texel.r;
                data[index * 4 + 1] = texel.g;
                data[index * 4 + 2] = texel.b;
                data[index * 4 + 3] = texel.a;
            }
        }

        // Set format for RGBA
        format = GL_RGBA;
        internalFormat = GL_SRGB_ALPHA;

        // Set texture wrapping for RGBA (always use CLAMP_TO_EDGE for RGBA)
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        // Upload texture data to GPU
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, mWidth, mHeight, 0, format, GL_UNSIGNED_BYTE, data));

        // Generate mipmaps for the texture
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
    }

    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0)); // Unbind the texture
        if (aiTex->mHeight == 0)
        {
            stbi_image_free(data);
        }
        else
        {
            delete[] data;
        }
        throw std::runtime_error("OpenGL error after uploading texture: " + std::to_string(error));
    }

    // Unbind the texture
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

    // Free the CPU-side data
    if (aiTex->mHeight == 0)
    {
        stbi_image_free(data);
    }
    else
    {
        delete[] data;
    }
}

// Create a texture from an existing OpenGL texture ID
Texture::Texture(GLuint textureID, int width, int height, Type type)
    : mTextureID(textureID), mWidth(width), mHeight(height), mType(type)
{
    // We're just wrapping an existing texture, no need to initialize anything
}

// Destructor - clean up GL resources
Texture::~Texture()
{
    if (mTextureID != 0)
    {
        GL_CHECK(glDeleteTextures(1, &mTextureID));
        mTextureID = 0;
    }
}

// Move constructor
Texture::Texture(Texture&& other) noexcept
    : mTextureID(other.mTextureID)
    , mWidth(other.mWidth)
    , mHeight(other.mHeight)
    , mType(other.mType)
    , mIsShadowSampler(other.mIsShadowSampler)
    , mIsNormalMap(other.mIsNormalMap)
    , mFormat(other.mFormat)
{
    // Null out the other texture ID to prevent double deletion
    other.mTextureID = 0;
}

// Move assignment operator
Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other)
    {
        // Clean up existing resources
        if (mTextureID != 0)
        {
            GL_CHECK(glDeleteTextures(1, &mTextureID));
        }

        // Move resources from other
        mTextureID = other.mTextureID;
        mWidth = other.mWidth;
        mHeight = other.mHeight;
        mType = other.mType;
        mIsShadowSampler = other.mIsShadowSampler;
        mIsNormalMap = other.mIsNormalMap;
        mFormat = other.mFormat;

        // Null out the other texture ID
        other.mTextureID = 0;
    }
    return *this;
}

// Bind the texture to a specified texture slot
void Texture::bind(unsigned int slot)
{
    if (slot >= 32) // OpenGL guarantees at least 32 texture units
    {
        LOG_WARN(logging::gGraphicsLogger, "Texture slot {} is out of range (0-31).", slot);
        return;
    }
    
    // First unbind anything in this slot
    unbind(slot);
    
    // Then bind our texture to the appropriate target
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
    GLenum target = toGLTextureTarget(mType);
    GL_CHECK(glBindTexture(target, mTextureID));
}

// Unbind any texture from a specified texture slot
void Texture::unbind(unsigned int slot)
{
    if (slot >= 32) // OpenGL guarantees at least 32 texture units
    {
        LOG_WARN(logging::gGraphicsLogger, "Texture slot {} is out of range (0-31).", slot);
        return;
    }
    
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

// Configure texture as a shadow sampler
void Texture::setShadowSamplerParameters()
{
    bind(31); // Use a consistent slot for configuration
    
    GLenum target = toGLTextureTarget(mType);
    
    if (mType == Type::TEXTURE_2D)
    {
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
        
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GL_CHECK(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));
    }
    else if (mType == Type::CUBEMAP)
    {
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
    }
    
    unbind(31);
    mIsShadowSampler = true;
}

// Configure texture as a normal map
void Texture::setNormalSamplerParameters()
{
    bind(31); // Use a consistent slot for configuration
    
    GLenum target = toGLTextureTarget(mType);
    
    if (mType == Type::TEXTURE_2D)
    {
        // Disable depth comparison mode
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    }
    else if (mType == Type::CUBEMAP)
    {
        // For cubemaps, we still set them as a type of shadow sampler
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    }
    
    unbind(31);
    mIsShadowSampler = (mType == Type::CUBEMAP); // Only cubemaps are set as shadow samplers
    mIsNormalMap = true;
}

// Set texture filtering modes
void Texture::setFilterMode(FilterMode minFilter, FilterMode magFilter)
{
    bind(31); // Use a consistent slot for configuration
    
    GLenum target = toGLTextureTarget(mType);
    GL_CHECK(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, toGLFilterMode(minFilter)));
    GL_CHECK(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, toGLFilterMode(magFilter)));
    
    unbind(31);
}

// Set texture wrapping modes
void Texture::setWrapMode(WrapMode wrapS, WrapMode wrapT, WrapMode wrapR)
{
    bind(31); // Use a consistent slot for configuration
    
    GLenum target = toGLTextureTarget(mType);
    GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_S, toGLWrapMode(wrapS)));
    GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_T, toGLWrapMode(wrapT)));
    
    if (mType == Type::CUBEMAP)
    {
        GL_CHECK(glTexParameteri(target, GL_TEXTURE_WRAP_R, toGLWrapMode(wrapR)));
    }
    
    unbind(31);
}

// Set compare mode for shadow sampling
void Texture::setCompareMode(CompareMode mode, CompareFunc func)
{
    bind(31); // Use a consistent slot for configuration
    
    GLenum target = toGLTextureTarget(mType);
    
    if (mode == CompareMode::CompareRefToTexture)
    {
        GL_CHECK(glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
        GL_CHECK(glTexParameteri(target, GL_TEXTURE_COMPARE_FUNC, toGLCompareFunc(func)));
        mIsShadowSampler = true;
    }
    else
    {
        GL_CHECK(glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_NONE));
        mIsShadowSampler = false;
    }
    
    unbind(31);
}

// Set border color for WrapMode::ClampToBorder
void Texture::setBorderColor(const glm::vec4& color)
{
    bind(31); // Use a consistent slot for configuration
    
    GLenum target = toGLTextureTarget(mType);
    float borderColor[] = { color.r, color.g, color.b, color.a };
    GL_CHECK(glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor));
    
    unbind(31);
}

// Generate mipmaps for the texture
void Texture::generateMipmaps()
{
    bind(31); // Use a consistent slot for configuration
    
    GLenum target = toGLTextureTarget(mType);
    GL_CHECK(glGenerateMipmap(target));
    
    unbind(31);
}

// Static helper methods

// Create an empty 2D texture
GLuint Texture::createEmptyTexture2D(int width, int height, Format format)
{
    GLuint textureID;
    GL_CHECK(glGenTextures(1, &textureID));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID));
    
    GLenum glInternalFormat = toGLInternalFormat(format);
    GLenum glFormat = toGLFormat(format);
    GLenum glType = toGLType(format);
    
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, glFormat, glType, nullptr));
    
    // Set default parameters
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
    
    return textureID;
}

// Create an empty cubemap texture
GLuint Texture::createEmptyCubemap(int size, Format format)
{
    GLuint textureID;
    GL_CHECK(glGenTextures(1, &textureID));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, textureID));
    
    GLenum glInternalFormat = toGLInternalFormat(format);
    GLenum glFormat = toGLFormat(format);
    GLenum glType = toGLType(format);
    
    for (unsigned int i = 0; i < 6; ++i)
    {
        GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, glInternalFormat, 
                          size, size, 0, glFormat, glType, nullptr));
    }
    
    // Set default parameters
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
    
    return textureID;
}

// Convert enum values to OpenGL constants
GLenum Texture::toGLInternalFormat(Format format)
{
    switch (format)
    {
        case Format::R:          return GL_R8;
        case Format::RG:         return GL_RG8;
        case Format::RGB:        return GL_RGB8;
        case Format::RGBA:       return GL_RGBA8;
        case Format::RGB16F:     return GL_RGB16F;
        case Format::RGBA16F:    return GL_RGBA16F;
        case Format::R32F:       return GL_R32F;
        case Format::Depth:      return GL_DEPTH_COMPONENT24;
        case Format::DepthStencil: return GL_DEPTH24_STENCIL8;
        default:                 return GL_RGBA8;
    }
}

GLenum Texture::toGLFormat(Format format)
{
    switch (format)
    {
        case Format::R:          return GL_RED;
        case Format::RG:         return GL_RG;
        case Format::RGB:        return GL_RGB;
        case Format::RGBA:       return GL_RGBA;
        case Format::RGB16F:     return GL_RGB;
        case Format::RGBA16F:    return GL_RGBA;
        case Format::R32F:       return GL_RED;
        case Format::Depth:      return GL_DEPTH_COMPONENT;
        case Format::DepthStencil: return GL_DEPTH_STENCIL;
        default:                 return GL_RGBA;
    }
}

GLenum Texture::toGLType(Format format)
{
    switch (format)
    {
        case Format::R:          return GL_UNSIGNED_BYTE;
        case Format::RG:         return GL_UNSIGNED_BYTE;
        case Format::RGB:        return GL_UNSIGNED_BYTE;
        case Format::RGBA:       return GL_UNSIGNED_BYTE;
        case Format::RGB16F:     return GL_FLOAT;
        case Format::RGBA16F:    return GL_FLOAT;
        case Format::R32F:       return GL_FLOAT;
        case Format::Depth:      return GL_FLOAT;
        case Format::DepthStencil: return GL_UNSIGNED_INT_24_8;
        default:                 return GL_UNSIGNED_BYTE;
    }
}

GLenum Texture::toGLFilterMode(FilterMode mode)
{
    switch (mode)
    {
        case FilterMode::Nearest:             return GL_NEAREST;
        case FilterMode::Linear:              return GL_LINEAR;
        case FilterMode::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
        case FilterMode::LinearMipmapNearest: return GL_LINEAR_MIPMAP_NEAREST;
        case FilterMode::NearestMipmapLinear: return GL_NEAREST_MIPMAP_LINEAR;
        case FilterMode::LinearMipmapLinear:  return GL_LINEAR_MIPMAP_LINEAR;
        default:                             return GL_LINEAR;
    }
}

GLenum Texture::toGLWrapMode(WrapMode mode)
{
    switch (mode)
    {
        case WrapMode::Repeat:         return GL_REPEAT;
        case WrapMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
        case WrapMode::ClampToEdge:    return GL_CLAMP_TO_EDGE;
        case WrapMode::ClampToBorder:  return GL_CLAMP_TO_BORDER;
        default:                      return GL_REPEAT;
    }
}

GLenum Texture::toGLCompareFunc(CompareFunc func)
{
    switch (func)
    {
        case CompareFunc::LessEqual:     return GL_LEQUAL;
        case CompareFunc::GreaterEqual:  return GL_GEQUAL;
        case CompareFunc::Less:          return GL_LESS;
        case CompareFunc::Greater:       return GL_GREATER;
        case CompareFunc::Equal:         return GL_EQUAL;
        case CompareFunc::NotEqual:      return GL_NOTEQUAL;
        case CompareFunc::Always:        return GL_ALWAYS;
        case CompareFunc::Never:         return GL_NEVER;
        default:                        return GL_LEQUAL;
    }
}

GLenum Texture::toGLTextureTarget(Type type)
{
    switch (type)
    {
        case Type::TEXTURE_2D:      return GL_TEXTURE_2D;
        case Type::CUBEMAP: return GL_TEXTURE_CUBE_MAP;
        default:                  return GL_TEXTURE_2D;
    }
}