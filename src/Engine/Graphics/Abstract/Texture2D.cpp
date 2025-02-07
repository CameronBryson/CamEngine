#include "pch.hpp"
#include "Texture2D.hpp"
#include "platform.hpp"
#include "OpenGLTexture2D.hpp"

std::shared_ptr<Texture2D> Texture2D::createTexture2D(const std::string& file)
{
	return std::make_shared<OpenGLTexture2D>(file);
}

std::shared_ptr<Texture2D> Texture2D::createTexture2D(const aiTexture* aiTex)
{
	return std::make_shared<OpenGLTexture2D>(aiTex);
}

std::shared_ptr<Texture2D> Texture2D::createTexture2D(GLuint textureID, int width, int height)
{
	return std::make_shared<OpenGLTexture2D>(textureID, width, height);
}
