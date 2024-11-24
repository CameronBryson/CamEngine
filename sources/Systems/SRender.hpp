#pragma once
#include "Components.hpp"
template<typename T>
class SparseSet;
class BaseScene;
class ShaderProgram;

class SRender
{
public:
    SRender(BaseScene* scene);
    void init();
    void render();
    void shutdown();

private:
    void drawModels(SparseSet<CTransform>& transforms, const ShaderProgram& shader) const;
    void drawColliders(SparseSet<CTransform>& transforms, const ShaderProgram& shader) const;
    void drawUi(SparseSet<CTransform>& transforms, const ShaderProgram& shader) const;
    void loadShaders() const;
    BaseScene* mScene;
};
