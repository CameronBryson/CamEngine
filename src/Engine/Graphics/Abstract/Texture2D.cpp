#include "pch.hpp"
#include "Texture2D.hpp"

std::shared_ptr<Texture2D> Texture2D::createTexture2D(const std::string& file, aiTextureType type)
{
	return std::shared_ptr<Texture2D>();
}

std::shared_ptr<Texture2D> Texture2D::createTexture2D(const aiTexture* aiTex, aiTextureType type)
{
	return std::shared_ptr<Texture2D>();
}
