#pragma once
#include "Engine/Components.hpp"
#include "glm/fwd.hpp"
template<typename T>
class SparseSet;
class BaseScene;
class Shader;

class SRender
{
public:
    SRender(BaseScene* scene);
    void init();
    void render();
    void shutdown();

private:
    void drawModels(SparseSet<CTransform>& transforms, const Shader& shader) const;
    void drawColliders(SparseSet<CTransform>& transforms, const Shader& shader) const;
    void drawUi(SparseSet<CTransform>& transforms, const Shader& shader) const;
    void loadShaders() const;
    BaseScene* mScene;
};
