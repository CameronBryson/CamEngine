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

// OpenGL Wrapper Methods Implementation
void Texture::genTextures(unsigned int count, unsigned int* textureIDs) {
    GL_CHECK(glGenTextures(count, textureIDs));
}

void Texture::deleteTextures(unsigned int count, const unsigned int* textureIDs) {
    GL_CHECK(glDeleteTextures(count, textureIDs));
}

void Texture::bindTexture(unsigned int target, unsigned int textureID) {
    GL_CHECK(glBindTexture(target, textureID));
}

void Texture::activeTexture(unsigned int textureUnit) {
    GL_CHECK(glActiveTexture(textureUnit));
}

void Texture::texImage2D(unsigned int target, int level, int internalFormat, 
                        int width, int height, int border, unsigned int format, 
                        unsigned int type, const void* data) {
    GL_CHECK(glTexImage2D(target, level, internalFormat, width, height, border, format, type, data));
}

void Texture::texImage2DMultisample(unsigned int target, int samples, int internalFormat,
                                   int width, int height, bool fixedSampleLocations) {
    GL_CHECK(glTexImage2DMultisample(target, samples, internalFormat, width, height, fixedSampleLocations));
}

void Texture::genMipmap(unsigned int target) {
    GL_CHECK(glGenerateMipmap(target));
}

void Texture::texParameteri(unsigned int target, unsigned int pname, int param) {
    GL_CHECK(glTexParameteri(target, pname, param));
}

void Texture::texParameterfv(unsigned int target, unsigned int pname, const float* params) {
    GL_CHECK(glTexParameterfv(target, pname, params));
}

void Texture::framebufferTexture(unsigned int target, unsigned int attachment, 
                                unsigned int texture, int level) {
    GL_CHECK(glFramebufferTexture(target, attachment, texture, level));
}

void Texture::framebufferTexture2D(unsigned int target, unsigned int attachment, 
                                  unsigned int textarget, unsigned int texture, int level) {
    GL_CHECK(glFramebufferTexture2D(target, attachment, textarget, texture, level));
}

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
    genTextures(1, &mTextureID);
    if (mTextureID == 0)
    {
        throw std::runtime_error("Failed to generate texture ID.");
    }
    bindTexture(GL_TEXTURE_2D, mTextureID);

    // Load the texture data using stb_image
    int nrChannels;
    stbi_set_flip_vertically_on_load(false); // Don't flip, maintain your current convention
    std::string fullPath = filePath;
    unsigned char* data = stbi_load(fullPath.c_str(), &mWidth, &mHeight, &nrChannels, 0);
    if (!data)
    {
        bindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
        deleteTextures(1, &mTextureID);
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

    // Upload the texture data to the GPU
    texImage2D(GL_TEXTURE_2D, 0, internalFormat, mWidth, mHeight, 0, format, GL_UNSIGNED_BYTE, data);

    // Set texture parameters using class methods (now unbind and use proper methods)
    bindTexture(GL_TEXTURE_2D, 0);

    // Set proper wrap mode based on format
    setWrapMode(format == GL_RGBA ? WrapMode::ClampToEdge : WrapMode::Repeat,
               format == GL_RGBA ? WrapMode::ClampToEdge : WrapMode::Repeat);
    setFilterMode(FilterMode::LinearMipmapLinear, FilterMode::Linear);
    
    // Generate mipmaps using our method
    generateMipmaps();

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
    genTextures(1, &hdrTexID);
    bindTexture(GL_TEXTURE_2D, hdrTexID);
    texImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, hdrData);
    texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(hdrData);

    genTextures(1, &mTextureID);
    bindTexture(GL_TEXTURE_CUBE_MAP, mTextureID);
    for (unsigned int i = 0; i < 6; ++i)
    {
        texImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                  ENV_MAP_SIZE, ENV_MAP_SIZE, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
    activeTexture(GL_TEXTURE0);
    bindTexture(GL_TEXTURE_2D, hdrTexID);

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

    deleteTextures(1, &hdrTexID);
}

// Create a texture from an Assimp texture object
Texture::Texture(const aiTexture* aiTex)
    : mTextureID(0), mWidth(0), mHeight(0), mType(Type::TEXTURE_2D)
{
    // Generate and bind the texture
    genTextures(1, &mTextureID);
    if (mTextureID == 0)
    {
        throw std::runtime_error("Failed to generate texture ID.");
    }
    bindTexture(GL_TEXTURE_2D, mTextureID);

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

            // Upload the texture data to the GPU
            texImage2D(GL_TEXTURE_2D, 0, internalFormat, mWidth, mHeight, 0, format, GL_UNSIGNED_BYTE, data);
            bindTexture(GL_TEXTURE_2D, 0); // Unbind before using our methods
            
            // Set texture parameters using class methods
            setWrapMode(format == GL_RGBA ? WrapMode::ClampToEdge : WrapMode::Repeat,
                       format == GL_RGBA ? WrapMode::ClampToEdge : WrapMode::Repeat);
            setFilterMode(FilterMode::LinearMipmapLinear, FilterMode::Linear);
            
            // Generate mipmaps using our method
            generateMipmaps();
        }
        else
        {
            LOG_ERROR(logging::gGraphicsLogger, "Failed to load embedded compressed texture.");
            bindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
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

        // Upload texture data to GPU
        texImage2D(GL_TEXTURE_2D, 0, internalFormat, mWidth, mHeight, 0, format, GL_UNSIGNED_BYTE, data);
        bindTexture(GL_TEXTURE_2D, 0); // Unbind before using our methods
        
        // Set texture parameters using class methods
        setWrapMode(WrapMode::ClampToEdge, WrapMode::ClampToEdge);
        setFilterMode(FilterMode::LinearMipmapLinear, FilterMode::Linear);
        
        // Generate mipmaps using our method
        generateMipmaps();
    }

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
        deleteTextures(1, &mTextureID);
        mTextureID = 0;
    }
}

// Move constructor
Texture::Texture(Texture&& other) noexcept
    : mTextureID(other.mTextureID)
    , mWidth(other.mWidth)
    , mHeight(other.mHeight)
    , mType(other.mType)
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
            deleteTextures(1, &mTextureID);
        }

        // Move resources from other
        mTextureID = other.mTextureID;
        mWidth = other.mWidth;
        mHeight = other.mHeight;
        mType = other.mType;
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
    activeTexture(GL_TEXTURE0 + slot);
    GLenum target = toGLTextureTarget(mType);
    bindTexture(target, mTextureID);
}

// Unbind any texture from a specified texture slot
void Texture::unbind(unsigned int slot)
{
    if (slot >= 32) // OpenGL guarantees at least 32 texture units
    {
        LOG_WARN(logging::gGraphicsLogger, "Texture slot {} is out of range (0-31).", slot);
        return;
    }
    
    activeTexture(GL_TEXTURE0 + slot);
    bindTexture(GL_TEXTURE_2D, 0);
    bindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

// Configure texture as a shadow sampler
void Texture::setShadowSamplerParameters()
{
    bind(31); // Use a consistent slot for configuration
    
    GLenum target = toGLTextureTarget(mType);
    
    if (mType == Type::TEXTURE_2D)
    {
        texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        texParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        texParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        texParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
    else if (mType == Type::CUBEMAP)
    {
        texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    }
    
    unbind(31);
}

// Configure texture as a normal map
void Texture::setNormalSamplerParameters()
{
    bind(31); // Use a consistent slot for configuration
    
    GLenum target = toGLTextureTarget(mType);
    
    if (mType == Type::TEXTURE_2D)
    {
        // Disable depth comparison mode
        texParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else if (mType == Type::CUBEMAP)
    {
        // For cubemaps, we still set them as a type of shadow sampler
        texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    
    unbind(31);
}

// Set texture filtering modes
void Texture::setFilterMode(FilterMode minFilter, FilterMode magFilter)
{
    bind(31); // Use a consistent slot for configuration
    
    GLenum target = toGLTextureTarget(mType);
    texParameteri(target, GL_TEXTURE_MIN_FILTER, toGLFilterMode(minFilter));
    texParameteri(target, GL_TEXTURE_MAG_FILTER, toGLFilterMode(magFilter));
    
    unbind(31);
}

// Set texture wrapping modes
void Texture::setWrapMode(WrapMode wrapS, WrapMode wrapT, WrapMode wrapR)
{
    bind(31); // Use a consistent slot for configuration
    
    GLenum target = toGLTextureTarget(mType);
    texParameteri(target, GL_TEXTURE_WRAP_S, toGLWrapMode(wrapS));
    texParameteri(target, GL_TEXTURE_WRAP_T, toGLWrapMode(wrapT));
    
    if (mType == Type::CUBEMAP)
    {
        texParameteri(target, GL_TEXTURE_WRAP_R, toGLWrapMode(wrapR));
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
        texParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        texParameteri(target, GL_TEXTURE_COMPARE_FUNC, toGLCompareFunc(func));
    }
    else
    {
        texParameteri(target, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    }
    
    unbind(31);
}

// Set border color for WrapMode::ClampToBorder
void Texture::setBorderColor(const glm::vec4& color)
{
    bind(31); // Use a consistent slot for configuration
    
    GLenum target = toGLTextureTarget(mType);
    float borderColor[] = { color.r, color.g, color.b, color.a };
    texParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    unbind(31);
}

// Generate mipmaps for the texture
void Texture::generateMipmaps()
{
    bind(31); // Use a consistent slot for configuration
    
    GLenum target = toGLTextureTarget(mType);
    genMipmap(target);
    
    unbind(31);
}

// Static helper methods

// Create an empty 2D texture
GLuint Texture::createEmptyTexture2D(int width, int height, Format format)
{
    GLuint textureID;
    genTextures(1, &textureID);
    bindTexture(GL_TEXTURE_2D, textureID);
    
    GLenum glInternalFormat = toGLInternalFormat(format);
    GLenum glFormat = toGLFormat(format);
    GLenum glType = toGLType(format);
    
    texImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, glFormat, glType, nullptr);
    
    // Set default parameters directly since we don't have a Texture instance yet
    texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    bindTexture(GL_TEXTURE_2D, 0);
    
    return textureID;
}

// Create an empty cubemap texture
GLuint Texture::createEmptyCubemap(int size, Format format)
{
    GLuint textureID;
    genTextures(1, &textureID);
    bindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    
    GLenum glInternalFormat = toGLInternalFormat(format);
    GLenum glFormat = toGLFormat(format);
    GLenum glType = toGLType(format);
    
    for (unsigned int i = 0; i < 6; ++i)
    {
        texImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, glInternalFormat, 
                  size, size, 0, glFormat, glType, nullptr);
    }
    
    // Set default parameters directly since we don't have a Texture instance yet
    texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    texParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    bindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
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