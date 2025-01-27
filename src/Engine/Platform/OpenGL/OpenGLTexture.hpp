#pragma once
#include <string>

#include "assimp/material.h"
#include "Engine/Graphics/Texture.hpp"
#include "Engine/Util/platform.hpp"
class aiTexture;
class OpenGLTexture : public Texture
{
public:
	explicit OpenGLTexture(const std::string& file, aiTextureType type);
	explicit OpenGLTexture(const aiTexture* aiTex, aiTextureType type);
	~OpenGLTexture();
	void bind(unsigned int slot) const override;
	void unbind(unsigned int slot) override;
	void deleteTexture() const override;
	int getWidth() const override;
	int getHeight() const override;
	unsigned char* getData() const override;

private:
	GLuint textureID = 0;
	unsigned char* data = nullptr;
	int width = 0;
	int height = 0;
	aiTextureType type;
};