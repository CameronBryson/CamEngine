#pragma once
#include "Components.hpp"
#include "Engine/SparseSet.hpp"
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
    void drawModels(SparseSet<CTransform>& transforms, ShaderProgram& shader);
    void drawColliders(SparseSet<CTransform>& transforms, ShaderProgram& shader);
    void drawUi(SparseSet<CTransform>& transforms, ShaderProgram& shader);
    void loadShaders();
    BaseScene* mScene;
};
