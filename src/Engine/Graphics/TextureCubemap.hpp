#pragma once

#include <memory>
#include <string>
#include "platform.hpp"
#include <Texture.hpp>

class Shader;
class TextureCubemap : public Texture
{
public:
	TextureCubemap(const std::string& hdrPath,
						 const std::shared_ptr<Shader>& equirectShader);

	TextureCubemap(GLuint id, int width, int height);

	~TextureCubemap() override;

	unsigned int  getTextureID() const  override { return mTextureID; }

	void bind(unsigned int slot)override;
	void unbind(unsigned int slot) override;
	int  getWidth()  const override;
	int  getHeight() const override;
	void setShadowSamplerParameters() override;
	void setNormalSamplerParameters() override;

private:
	GLuint mTextureID = 0;
	int    mWidth = 0;
	int    mHeight = 0;
	bool  mIsShadowSampler = false;

};
