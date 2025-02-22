#include "pch.hpp"
#include "OpenGLTexture2D.hpp"

#include <stdexcept>
#include <iostream>
#include "Engine/Util/EngineUtil.hpp"
#include "stb_image.h"
#include <glad/glad.h>
#include <assimp/texture.h>
#include "OpenGLUtil.hpp" // Include the header for GL_CHECK macro

// Constructor for loading textures from a file
OpenGLTexture2D::OpenGLTexture2D(const std::string& file)
    : textureID(0), width(0), height(0)
{
    // Generate and bind the texture
    GL_CHECK(glGenTextures(1, &textureID));
    if (textureID == 0)
    {
        throw std::runtime_error("Failed to generate texture ID.");
    }
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID));

    // Load the texture data using stb_image
    int nrChannels;
    stbi_set_flip_vertically_on_load(false); // Flip the image vertically if needed
    std::string fullPath = engine_util::buildPath(file);
    unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);
    if (!data)
    {
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0)); // Unbind the texture
        throw std::runtime_error("Failed to load texture: " + file);
    }
    std::cout << "Loaded texture: " << file << std::endl;

    // Determine the texture format
    GLenum format;
    GLenum internalFormat;

    if (nrChannels == 1)
    {
        format = GL_RED;
        internalFormat = GL_RED;
    }
    else if (nrChannels == 3)
    {
        format = GL_RGB;
        internalFormat = GL_SRGB; // Assume diffuse textures are sRGB
    }
    else if (nrChannels == 4)
    {
        format = GL_RGBA;
        internalFormat = GL_SRGB_ALPHA; // Assume diffuse textures are sRGB
    }
    else
    {
        std::cerr << "Unknown format: " << nrChannels << " channels. Defaulting to GL_RGB." << std::endl;
        format = GL_RGB;
        internalFormat = GL_RGB8;
    }

    // Set texture wrapping based on format
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    // Upload the texture data to the GPU
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data));

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

// Constructor for loading embedded textures from aiTexture
OpenGLTexture2D::OpenGLTexture2D(const aiTexture* aiTex)
    : textureID(0), width(0), height(0)
{
    // Generate and bind the texture
    GL_CHECK(glGenTextures(1, &textureID));
    if (textureID == 0)
    {
        throw std::runtime_error("Failed to generate texture ID.");
    }
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID));

    unsigned char* data = nullptr;
    GLenum format = GL_RGB;
    GLenum internalFormat = GL_RGB8;

    if (aiTex->mHeight == 0)
    {
        // Compressed texture (e.g., PNG, JPEG)
        int nrChannels;
        stbi_set_flip_vertically_on_load(false); // Flip the image vertically if needed
        data = stbi_load_from_memory(
            reinterpret_cast<const stbi_uc*>(aiTex->pcData),
            static_cast<int>(aiTex->mWidth),
            &width,
            &height,
            &nrChannels,
            0
        );

        if (data)
        {
            if (nrChannels == 1)
            {
                format = GL_RED;
                internalFormat = GL_RED;
            }
            else if (nrChannels == 3)
            {
                format = GL_RGB;
                internalFormat = GL_SRGB;
            }
            else if (nrChannels == 4)
            {
                format = GL_RGBA;
                internalFormat = GL_SRGB_ALPHA;
            }
            else
            {
                format = GL_RGB; // Fallback
                internalFormat = GL_RGB8;
            }

            // Set texture wrapping based on format
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT));
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT));
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

            // Upload the texture data to the GPU
            GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data));

            // Generate mipmaps for the texture
            GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
        }
        else
        {
            std::cerr << "Failed to load embedded compressed texture." << std::endl;
            GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0)); // Unbind the texture
            throw std::runtime_error("Failed to load embedded compressed texture.");
        }
    }
    else
    {
        // Uncompressed texture (RGBA8888 format)
        width = static_cast<int>(aiTex->mWidth);
        height = static_cast<int>(aiTex->mHeight);

        // Allocate memory for texture data
        data = new unsigned char[width * height * 4]; // 4 channels (RGBA)

        // Copy data from aiTex->pcData (which is in aiTexel format)
        for (unsigned int y = 0; y < static_cast<unsigned int>(height); ++y)
        {
            for (unsigned int x = 0; x < static_cast<unsigned int>(width); ++x)
            {
                unsigned int index = y * width + x;
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
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data));

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

OpenGLTexture2D::OpenGLTexture2D(GLuint textureID, int width, int height)
{
    this->textureID = textureID;
    this->width = width;
    this->height = height;
}

// Destructor
OpenGLTexture2D::~OpenGLTexture2D()
{
    //Delete here
}

// Bind the texture to a specified texture slot
void OpenGLTexture2D::bind(unsigned int slot)
{
    if (slot >= 32) // OpenGL guarantees at least 32 texture units
    {
        std::cerr << "Texture slot " << slot << " is out of range (0-31)." << std::endl;
        return;
    }
    unbind(slot);
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID));
}

// Unbind the texture from a specified texture slot
void OpenGLTexture2D::unbind(unsigned int slot) 
{
    if (slot >= 32) // OpenGL guarantees at least 32 texture units
    {
        std::cerr << "Texture slot " << slot << " is out of range (0-31)." << std::endl;
        return;
    }
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

// Get the width of the texture
int OpenGLTexture2D::getWidth() const
{
    return width;
}

// Get the height of the texture
int OpenGLTexture2D::getHeight() const
{
    return height;
}

void OpenGLTexture2D::setShadowSamplerParameters()
{
    bind(0);  // This could be the issue - binding to unit 0
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    unbind(0);
    isShadowSampler = true;
}



void OpenGLTexture2D::setNormalSamplerParameters()
{
    bind(0);
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    unbind(0);
    isShadowSampler = false;
}

