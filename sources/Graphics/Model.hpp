#pragma once
#include <vector>
#include <string>
#include "GraphicsManager.hpp"
class Model
{
public:
  explicit Model(const std::vector<std::string>& meshes);

  void draw(ShaderProgram& shader, GraphicsManager& graphics_manager);

  void addMesh(const std::string& mesh_name);
private:
    std::vector<std::string> meshes;
};
