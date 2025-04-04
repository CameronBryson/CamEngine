#pragma once
#include <string>

#include "Texture.hpp"
#include "Engine/Util/platform.hpp"
#include <assimp/texture.h>
class Texture2D : public Texture
{
public:
	Texture2D(const std::string& file);
	Texture2D(const aiTexture* aiTex);
	Texture2D(GLuint textureID, int width, int height);
	~Texture2D();
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