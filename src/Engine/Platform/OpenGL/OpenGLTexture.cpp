#include "pch.hpp"
#include "OpenGLTexture.hpp"

#include <stdexcept>

#include "Engine/Util/EngineUtil.hpp"
#include "stb_image.h"

OpenGLTexture::OpenGLTexture(const std::string& file, aiTextureType type)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int nrChannels;
	data = stbi_load(engine_util::buildPath(file).c_str(), &width, &height, &nrChannels, 0);
	if (!data)
	{
		throw std::runtime_error("Failed to load texture: " + std::string(file));
	}
	printf("Loaded texture: %s\n", file.c_str());

	GLenum format;
	GLint internalFormat;

	if (nrChannels == 1)
	{
		format = GL_RED;
		internalFormat = GL_RED;
	}
	else if (nrChannels == 3)
	{
		format = GL_RGB;
		internalFormat = GL_RGB8;
	}
	else if (nrChannels == 4)
	{
		format = GL_RGBA;
		internalFormat = GL_RGBA8;
	}
	else
	{
		printf("Unknown format: %d channels\n", nrChannels);
		format = GL_RGB;          // Default to GL_RGB
		internalFormat = GL_RGB8; // Default internal format
	}
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);


	stbi_image_free(data);
}

#include <glad/glad.h>
#include <stb_image.h> // Ensure you have stb_image for image loading
#include <iostream>
#include <assimp/texture.h>

OpenGLTexture::OpenGLTexture(const aiTexture* aiTex, aiTextureType type)
    : type(type)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    if (aiTex->mHeight == 0)
    {
        // Compressed texture (e.g., PNG, JPEG)
        // Load the compressed image data using an image loader (e.g., stb_image)
        int channels;
        data = stbi_load_from_memory(
            reinterpret_cast<const stbi_uc*>(aiTex->pcData),
            static_cast<int>(aiTex->mWidth),
            &width,
            &height,
            &channels,
            0
        );

        if (data)
        {
            GLenum format;
            if (channels == 1)
                format = GL_RED;
            else if (channels == 3)
                format = GL_RGB;
            else if (channels == 4)
                format = GL_RGBA;
            else
                format = GL_RGB; // Fallback

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cerr << "Failed to load embedded compressed texture." << std::endl;
        }
    }
    else
    {
        // Uncompressed texture (RGBA8888 format)
        width = aiTex->mWidth;
        height = aiTex->mHeight;

        // Allocate memory for texture data
        data = new unsigned char[width * height * 4]; // 4 channels (RGBA)

        // Copy data from aiTex->pcData (which is in aiTexel format)
        for (unsigned int y = 0; y < height; ++y)
        {
            for (unsigned int x = 0; x < width; ++x)
            {
                unsigned int index = y * width + x;
                const aiTexel& texel = aiTex->pcData[index];
                data[index * 4 + 0] = texel.r;
                data[index * 4 + 1] = texel.g;
                data[index * 4 + 2] = texel.b;
                data[index * 4 + 3] = texel.a;
            }
        }

        // Upload texture data to GPU
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Texture parameters (you can adjust these as needed)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // or GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // or GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // or GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Free the CPU-side data if not needed
    // For compressed texture, stb_image handles deallocation
    if (aiTex->mHeight != 0)
    {
        delete[] data;
        data = nullptr;
    }
    else
    {
        stbi_image_free(data);
        data = nullptr;
    }
}


OpenGLTexture::~OpenGLTexture() { deleteTexture(); }

void OpenGLTexture::bind(unsigned int slot ) const
{ 
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, textureID); 
}

void OpenGLTexture::unbind(unsigned int slot) 
{ 
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, 0); 
}

void OpenGLTexture::deleteTexture() const { glDeleteTextures(1, &textureID); }

int OpenGLTexture::getWidth() const { return width; }

int OpenGLTexture::getHeight() const { return height; }

unsigned char* OpenGLTexture::getData() const { return data; }
