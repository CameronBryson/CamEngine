#include "Texture.hpp" 
texture::texture(const std::string& file)
{
    opengl_util::create_texture(file, data, texture_id);
}
void texture::bind() const
{
    opengl_util::bind_texture(texture_id);
}
void texture::unbind()
{
    opengl_util::unbind_texture();
}
void texture::delete_texture() const
{
    opengl_util::delete_texture(texture_id);
}
