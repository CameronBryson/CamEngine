#pragma once
#include <vector>
#include <string>

#include "Vertex.hpp"
class GraphicsManager;
class ShaderProgram;
class Mesh
{
public:
  explicit Mesh(const std::vector<Vertex>& vertices, const std::string& materialName);

  void setupMesh();

  void draw(const ShaderProgram& shader, GraphicsManager& graphicsManager) const;

  void setMaterial(const std::string& name);
private:
    unsigned int VAO = 0, VBO = 0, index_count;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::string material_name;
};
