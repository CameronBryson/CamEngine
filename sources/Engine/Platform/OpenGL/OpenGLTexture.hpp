#pragma once
#include <string>

#include "Engine/Graphics/Texture.hpp"
#include "Engine/Util/platform.hpp"
class OpenGLTexture : public Texture
{
public:
	explicit OpenGLTexture(const std::string& file);
	void bind() const override;
	void unbind() override;
	void deleteTexture() const override;
private:
	GLuint textureID = 0;
	unsigned char* data = nullptr;
};