#include "pch.hpp"
#include "TextureCubemap.hpp"
#include <OpenGLTextureCubemap.hpp>
#include "memory.h"
#include "Shader.hpp"

std::shared_ptr<TextureCubemap> TextureCubemap::createTextureCubemap(const std::string& file, const std::shared_ptr<Shader>& shader)
{
	return std::make_shared<OpenGLTextureCubemap>(file, shader);
}

