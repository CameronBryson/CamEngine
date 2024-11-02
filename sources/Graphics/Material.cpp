#include "Material.hpp"
Material::Material(const std::string& diffuse_path, const std::string& specular_path, float shininess, glm::vec3 ambient_color, glm::vec3 diffuse_color,
    glm::vec3 specular_color)
    : shininess(shininess), ambient_color(ambient_color), diffuse_color(diffuse_color), specular_color(specular_color)
{
    if( ! diffuse_path.empty() )
    {
	diffuse_texture = std::make_unique<Texture>(diffuse_path);
    }
    if( ! specular_path.empty() )
    {
	specular_texture = std::make_unique<Texture>(specular_path);
    }
}
void Material::bind(const ShaderProgram& shader) const
{
    OpenGlUtil::bindMaterial(shader, diffuse_texture.get(), specular_texture.get(), ambient_color, diffuse_color, specular_color, shininess);
}
void Material::unbind() const
{
    OpenGlUtil::unbindMaterial(diffuse_texture.get(), specular_texture.get());
}
float Material::getShininess() const
{
    return shininess;
}
