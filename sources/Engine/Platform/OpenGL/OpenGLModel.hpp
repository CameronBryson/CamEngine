#pragma once
#include <vector>
#include <string>

#include "Engine/Graphics/Model.hpp"
class Shader;
class GraphicsManager;
class OpenGLModel : public Model
{
public:
    explicit OpenGLModel(const std::vector<std::string>& meshes);

    void draw(const Shader& shader, GraphicsManager& graphicsManager) const override;

    void addMesh(const std::string& meshName) override;
private:
    std::vector<std::string> meshes;
};
