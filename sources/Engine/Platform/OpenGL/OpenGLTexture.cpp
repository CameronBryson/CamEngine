#include "Engine/pch.hpp"
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
