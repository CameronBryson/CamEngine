#include "pch.hpp"
#include "Texture.hpp" 
#include "Engine/Platform/OpenGL/OpenGLTexture.hpp"


std::shared_ptr<Texture> Texture::createTexture(const std::string& file, aiTextureType type)
{
	return std::make_shared<OpenGLTexture>(file, type);
}

std::shared_ptr<Texture> Texture::createEmbeddedTexture(const aiTexture* aiTex, aiTextureType type)
{
	return std::make_shared<OpenGLTexture>(aiTex, type);
}
