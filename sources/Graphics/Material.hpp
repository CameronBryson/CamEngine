#pragma once
#include <string>
#include <memory>
#include "Texture.hpp"
#include "platform.hpp"
#include "ShaderProgram.hpp"

class material
{
public:
    material(const std::string& diffuse_path, const std::string& specular_path, float shininess,
             glm::vec3 ambient_color, glm::vec3 diffuse_color, glm::vec3 specular_color)
        : diffuse_path(diffuse_path), specular_path(specular_path), shininess(shininess),
          ambient_color(ambient_color), diffuse_color(diffuse_color), specular_color(specular_color)
    {
        if (!diffuse_path.empty())
        {
            diffuse_texture = std::make_unique<texture>(diffuse_path.c_str(), true);
        }
        if (!specular_path.empty())
        {
            specular_texture = std::make_unique<texture>(specular_path.c_str(), true);
        }
    }

    void bind(shader_program& shader)
    {
        if (diffuse_texture)
        {
            shader.setInt("material.diffuse", 0);
            glActiveTexture(GL_TEXTURE0);
            diffuse_texture->bind();
        }
        else
        {
            shader.setVec3("material.diffuse", diffuse_color);
        }

        if (specular_texture)
        {
            shader.setInt("material.specular", 1);
            glActiveTexture(GL_TEXTURE1);
            specular_texture->bind();
        }
        else
        {
            shader.setVec3("material.specular", specular_color);
        }

        shader.setFloat("material.shininess", shininess);
        shader.setVec3("material.ambient", ambient_color);
    }
    void unbind()
    {
        if(diffuse_texture)
        {
            diffuse_texture->unbind();
        }
        if(specular_texture)
        {
            specular_texture->unbind();
        }
    }

    float get_shininess() const
    {
        return shininess;
    }

private:
    std::string diffuse_path;
    std::string specular_path;
    float shininess;
    glm::vec3 ambient_color;
    glm::vec3 diffuse_color;
    glm::vec3 specular_color;
    std::unique_ptr<texture> diffuse_texture;
    std::unique_ptr<texture> specular_texture;
};
