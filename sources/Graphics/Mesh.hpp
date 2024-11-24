#pragma once
#include <utility>

#include "OpenGLUtil.hpp"
#include "Vertex.hpp"
class GraphiscManager;
class Mesh
{
public:
  explicit Mesh(const std::vector<Vertex>& vertices, const std::string& material_name);

  void setupMesh();

  void draw(ShaderProgram& shader, GraphicsManager& graphics_manager) const;

  void setMaterial(const std::string& name);
private:
    unsigned int VAO = 0, VBO = 0, index_count;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::string material_name;
};
