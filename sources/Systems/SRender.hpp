#pragma once
#include "Engine/SparseSet.hpp"
class Camera;
class registry;
class shader_program;
class c_transform;

class s_render
{
public:
    void init();
    void update(const registry& registry, Camera& camera);
    void shutdown();

private:
    void draw_models(const registry& registry, sparse_set<c_transform>& transforms, shader_program& shader);
    void draw_colliders(const registry& registry, sparse_set<c_transform>& transforms, shader_program& shader);
    void draw_ui(const registry& registry, sparse_set<c_transform>& transforms, shader_program& shader);
    void load_shaders();
};
