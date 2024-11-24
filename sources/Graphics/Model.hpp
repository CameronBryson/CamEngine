#pragma once
#include <vector>
#include <string>
class ShaderProgram;
class GraphicsManager;
class Model
{
public:
  explicit Model(const std::vector<std::string>& meshes);

  void draw(const ShaderProgram& shader, GraphicsManager& graphicsManager) const;

  void addMesh(const std::string& meshName);
private:
    std::vector<std::string> meshes;
};
