#pragma once
#include <string>

#include "assimp/material.h"
#include "Engine/Graphics/Texture.hpp"
#include "Engine/Util/platform.hpp"
class OpenGLTexture : public Texture
{
public:
	explicit OpenGLTexture(const std::string& file, aiTextureType type);
	void bind() const override;
	void unbind() override;
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