#pragma once
#include <utility>

#include "OpenGLUtil.hpp"
#include "Vertex.hpp"
#include "GraphicsManager.hpp"
class mesh
{
public:
  mesh(const std::vector<vertex>& vertices, std::string material_name);

  void setup_mesh();

  void draw(shader_program& shader, graphics_manager& graphics_manager) const;

  void set_material(const std::string& name);
private:
    unsigned int VAO = 0, VBO = 0, index_count;
    std::vector<vertex> vertices;
    std::vector<unsigned int> indices;
    std::string material_name;
};
