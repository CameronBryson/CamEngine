#pragma once
#include <string>

#include "assimp/material.h"
#include "Texture2D.hpp"
#include "Engine/Util/platform.hpp"
class aiTexture;
class OpenGLTexture2D : public Texture2D
{
public:
	explicit OpenGLTexture2D(const std::string& file);
	explicit OpenGLTexture2D(const aiTexture* aiTex);
	explicit OpenGLTexture2D(GLuint textureID, int width, int height);
	~OpenGLTexture2D();
	void bind(unsigned int slot) override;
	void unbind(unsigned int slot) override;
	int getWidth() const override;
	int getHeight() const override;
	unsigned int getTextureID() const override { return textureID; }
	void setShadowSamplerParameters() override;
	void setNormalSamplerParameters() override;

private:
	GLuint textureID = 0;
	unsigned char* data = nullptr;
	int width = 0;
	int height = 0;
	bool isShadowSampler = false;
	bool isNormalMap = false;
};