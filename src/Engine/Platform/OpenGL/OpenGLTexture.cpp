// OpenGLTexture.cpp

#include "pch.hpp"
#include "OpenGLTexture.hpp"

#include <stdexcept>
#include <iostream>
#include "Engine/Util/EngineUtil.hpp"
#include "stb_image.h"
#include <glad/glad.h>
#include <assimp/texture.h>

// Constructor for loading textures from a file
OpenGLTexture::OpenGLTexture(const std::string& file, aiTextureType type)
    : textureID(0), width(0), height(0), type(type)
{
    // Generate and bind the texture
    glGenTextures(1, &textureID);
    if (textureID == 0)
    {
        throw std::runtime_error("Failed to generate texture ID.");
    }
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture wrapping and filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // You can change to GL_CLAMP_TO_EDGE if needed
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // You can change to GL_CLAMP_TO_EDGE if needed
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // For PBR, trilinear filtering is preferred
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load the texture data using stb_image
    int nrChannels;
    stbi_set_flip_vertically_on_load(false); // Flip the image vertically if needed
    std::string fullPath = engine_util::buildPath(file);
    unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);
    if (!data)
    {
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
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
        internalFormat = (type == aiTextureType_DIFFUSE || type == aiTextureType_BASE_COLOR) ? GL_SRGB : GL_RGB8;
    }
    else if (nrChannels == 4)
    {
        format = GL_RGBA;
        internalFormat = (type == aiTextureType_DIFFUSE || type == aiTextureType_BASE_COLOR) ? GL_SRGB_ALPHA : GL_RGBA8;
    }
    else
    {
        std::cerr << "Unknown format: " << nrChannels << " channels. Defaulting to GL_RGB." << std::endl;
        format = GL_RGB;
        internalFormat = GL_RGB8;
    }

    // Upload the texture data to the GPU
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    // Generate mipmaps for the texture
    glGenerateMipmap(GL_TEXTURE_2D);

    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
        stbi_image_free(data);
        throw std::runtime_error("OpenGL error after uploading texture: " + std::to_string(error));
    }

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Free the image data
    stbi_image_free(data);
}

// Constructor for loading embedded textures from aiTexture
OpenGLTexture::OpenGLTexture(const aiTexture* aiTex, aiTextureType type)
    : textureID(0), width(0), height(0), type(type)
{
    // Generate and bind the texture
    glGenTextures(1, &textureID);
    if (textureID == 0)
    {
        throw std::runtime_error("Failed to generate texture ID.");
    }
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture wrapping and filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // You can change to GL_CLAMP_TO_EDGE if needed
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // You can change to GL_CLAMP_TO_EDGE if needed
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // For PBR, trilinear filtering is preferred
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;
            else
                format = GL_RGB; // Fallback

            // Adjust internal format based on texture type for PBR
            if (type == aiTextureType_DIFFUSE || type == aiTextureType_BASE_COLOR)
            {
                if (nrChannels == 3)
                    internalFormat = GL_SRGB;
                else if (nrChannels == 4)
                    internalFormat = GL_SRGB_ALPHA;
            }
            else
            {
                internalFormat = (nrChannels == 4) ? GL_RGBA8 : GL_RGB8;
            }

            // Upload the texture data to the GPU
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

            // Generate mipmaps for the texture
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cerr << "Failed to load embedded compressed texture." << std::endl;
            glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
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

        // Set format
        format = GL_RGBA;
        internalFormat = (type == aiTextureType_DIFFUSE || type == aiTextureType_BASE_COLOR) ? GL_SRGB_ALPHA : GL_RGBA8;

        // Upload texture data to GPU
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        // Generate mipmaps for the texture
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
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
    glBindTexture(GL_TEXTURE_2D, 0);

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

// Destructor
OpenGLTexture::~OpenGLTexture()
{
    deleteTexture();
}

// Bind the texture to a specified texture slot
void OpenGLTexture::bind(unsigned int slot) const
{
    if (slot >= 32) // OpenGL guarantees at least 32 texture units
    {
        std::cerr << "Texture slot " << slot << " is out of range (0-31)." << std::endl;
        return;
    }
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

// Unbind the texture from a specified texture slot
void OpenGLTexture::unbind(unsigned int slot)
{
    if (slot >= 32) // OpenGL guarantees at least 32 texture units
    {
        std::cerr << "Texture slot " << slot << " is out of range (0-31)." << std::endl;
        return;
    }
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Delete the texture from GPU memory
void OpenGLTexture::deleteTexture() const
{
    if (textureID != 0)
    {
        glDeleteTextures(1, &textureID);
    }
}

// Get the width of the texture
int OpenGLTexture::getWidth() const
{
    return width;
}

// Get the height of the texture
int OpenGLTexture::getHeight() const
{
    return height;
}
