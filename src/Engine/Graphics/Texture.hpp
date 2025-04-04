#pragma once
class Texture
{
public:
	virtual ~Texture() = default;
	virtual void bind(unsigned int slot)= 0;
	virtual void unbind(unsigned int slot) = 0;
	virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;
	virtual unsigned int getTextureID() const = 0;

	virtual void setShadowSamplerParameters() = 0;
	virtual void setNormalSamplerParameters() = 0;
};