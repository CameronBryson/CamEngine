#include "Engine/pch.hpp"
#include "Texture.hpp" 

#include "Engine/Platform/OpenGL/OpenGLTexture.hpp"


std::shared_ptr<Texture> Texture::createTexture(const std::string& file, aiTextureType type)
{
	return std::make_shared<OpenGLTexture>(file, type);
}
