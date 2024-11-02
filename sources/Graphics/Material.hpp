#pragma once
#include <string>
#include <memory>

#include "OpenGLUtil.hpp"
#include "Texture.hpp"
#include "ShaderProgram.hpp"

class Material
{
public:
  Material(const std::string& diffuse_path, const std::string& specular_path, float shininess, glm::vec3 ambient_color, glm::vec3 diffuse_color,
      glm::vec3 specular_color);

  void bind(const ShaderProgram& shader) const;
  void unbind() const;

  [[nodiscard]] float getShininess() const;
private:
    float shininess;
    glm::vec3 ambient_color;
    glm::vec3 diffuse_color;
    glm::vec3 specular_color;
    std::unique_ptr<Texture> diffuse_texture;
    std::unique_ptr<Texture> specular_texture;
};
