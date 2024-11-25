#include "Texture.hpp" 

#include "Engine/Platform/OpenGL/OpenGLTexture.hpp"


std::shared_ptr<Texture> Texture::createTexture(const std::string& file)
{
	return std::make_shared<OpenGLTexture>(file);
}
