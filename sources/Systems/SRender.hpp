#pragma once
#include "Engine/SparseSet.hpp"
class Camera;
class Registry;
class ShaderProgram;
class CTransform;
class GraphicsManager;

class SRender
{
public:
    void init(GraphicsManager& graphics_manager);
    void update(const Registry& registry, GraphicsManager& graphics_manager, Camera& camera);
    void shutdown();

private:
    void drawModels(const Registry& registry, GraphicsManager& graphics_manager,SparseSet<CTransform>& transforms, ShaderProgram& shader);
    void drawColliders(const Registry& registry, GraphicsManager& graphics_manager, SparseSet<CTransform>& transforms, ShaderProgram& shader);
    void drawUi(const Registry& registry, GraphicsManager& graphics_manager, SparseSet<CTransform>& transforms, ShaderProgram& shader);
    void loadShaders(GraphicsManager& graphics_manager);
};
