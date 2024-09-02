#pragma once
#include <string>
#include <memory>

#include "OpenGLUtil.hpp"
#include "Texture.hpp"
#include "platform.hpp"
#include "ShaderProgram.hpp"

class material
{
public:
    material(const std::string& diffuse_path, const std::string& specular_path, float shininess,
             glm::vec3 ambient_color, glm::vec3 diffuse_color, glm::vec3 specular_color)
        : shininess(shininess),
          ambient_color(ambient_color), diffuse_color(diffuse_color), specular_color(specular_color)
    {
        if (!diffuse_path.empty())
        {
            diffuse_texture = std::make_unique<texture>(diffuse_path);
        }
        if (!specular_path.empty())
        {
            specular_texture = std::make_unique<texture>(specular_path);
        }
    }

    void bind(const shader_program& shader) const
    {
        opengl_util::bind_material(shader,diffuse_texture.get(),specular_texture.get(),ambient_color,diffuse_color, specular_color, shininess);
    }
    void unbind() const
    {
       opengl_util::unbind_material(diffuse_texture.get(),specular_texture.get());
    }

    [[nodiscard]] float get_shininess() const
    {
        return shininess;
    }

private:
    float shininess;
    glm::vec3 ambient_color;
    glm::vec3 diffuse_color;
    glm::vec3 specular_color;
    std::unique_ptr<texture> diffuse_texture;
    std::unique_ptr<texture> specular_texture;
};
