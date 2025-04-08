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
	unsigned int getTextureID() const override { return mTextureID; }
	void setShadowSamplerParameters() override;
	void setNormalSamplerParameters() override;

private:
	GLuint mTextureID = 0;
	unsigned char* mData = nullptr;
	int mWidth = 0;
	int mHeight = 0;
	bool mIsShadowSampler = false;
	bool mIsNormalMap = false;
};