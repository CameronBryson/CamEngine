#pragma once
#include <vector>
#include <string>
#include "GraphicsManager.hpp"
class model
{
public:
  explicit model(const std::vector<std::string>& meshes);

  void draw(shader_program& shader, graphics_manager& graphics_manager);

  void add_mesh(const std::string& mesh_name);
private:
    std::vector<std::string> meshes;
};
