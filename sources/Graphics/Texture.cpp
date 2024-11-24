#include "Texture.hpp" 

#include "OpenGLUtil.hpp"

Texture::Texture(const std::string& file)
{
    OpenGlUtil::createTexture(file, data, texture_id);
}
void Texture::bind() const
{
    OpenGlUtil::bindTexture(texture_id);
}
void Texture::unbind()
{
    OpenGlUtil::unbindTexture();
}
void Texture::deleteTexture() const
{
    OpenGlUtil::deleteTexture(texture_id);
}
