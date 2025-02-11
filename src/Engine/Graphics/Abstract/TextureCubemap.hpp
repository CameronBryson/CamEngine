#pragma once
#include "Shader.hpp"
#include "Texture.hpp"
#include <memory>
#include <string>
#include "platform.hpp"
class TextureCubemap : public Texture
{
public:
	static std::shared_ptr<TextureCubemap> createTextureCubemap(const std::string& file, const std::shared_ptr<Shader>& equirectangularToCubemapShader);
	static std::shared_ptr<TextureCubemap> createTextureCubemap(GLuint textureID, int width, int height);
};

