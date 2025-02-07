#pragma once
#include "Texture.hpp"
#include <memory>
#include <string>
class TextureCubemap : public Texture
{
public:
	static std::shared_ptr<TextureCubemap> createTextureCubemap(const std::string& file);
};

