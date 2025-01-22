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

    void drawModels(const Shader& shader) const;

    void drawUi(const Shader& shader) const;

private:
    BaseScene* mScene;
};
