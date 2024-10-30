#pragma once
#include "Engine/SparseSet.hpp"
class Camera;
class registry;
class shader_program;
class c_transform;
class graphics_manager;

class s_render
{
public:
    void init(graphics_manager& graphics_manager);
    void update(const registry& registry, graphics_manager& graphics_manager, Camera& camera);
    void shutdown();

private:
    void draw_models(const registry& registry, graphics_manager& graphics_manager,sparse_set<c_transform>& transforms, shader_program& shader);
    void draw_colliders(const registry& registry, graphics_manager& graphics_manager, sparse_set<c_transform>& transforms, shader_program& shader);
    void draw_ui(const registry& registry, graphics_manager& graphics_manager, sparse_set<c_transform>& transforms, shader_program& shader);
    void load_shaders(graphics_manager& graphics_manager);
};
