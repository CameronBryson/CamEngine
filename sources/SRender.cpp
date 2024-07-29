#include "SRender.hpp"

#include <string>

#include "Components.hpp"
#include "raylib.h"

#include "Stats.hpp"

void s_render::init()
{
    printf("Render init\n");
}

void s_render::update(const registry &registry, const Camera3D &camera)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode3D(camera);

    // Draw CTransform (Cubes)
    auto &positions = registry.get_sparse_set<c_transform>();
    auto ids = registry.get_entity_ids<c_transform>();

    // Draw CSphere
    auto &spheres = registry.get_sparse_set<c_sphere>();
    ids = registry.get_entity_ids<c_sphere>();
    for (const auto id : ids)
    {
        auto &[radius] = spheres.get_item(id);
        auto &[position, rotation, scale] = positions.get_item(id); // Assuming CTransform is also present
        DrawSphere({position.x, position.y, position.z}, radius, BLUE);
        DrawSphereWires({position.x, position.y, position.z}, radius, 16, 16,
                        DARKBLUE);
    }

    // Draw CAABB
    auto &aabbs = registry.get_sparse_set<c_aabb>();
    ids = registry.get_entity_ids<c_aabb>();
    for (const auto id : ids)
    {
        auto &[extents] = aabbs.get_item(id);
        auto &[position, rotation, scale] = positions.get_item(id); // Assuming CTransform is also present
        DrawCube({position.x, position.y, position.z}, extents.x, extents.y,
                 extents.z, GREEN);
        DrawCubeWires({position.x, position.y, position.z}, extents.x, extents.y,
                      extents.z, DARKGREEN);
    }

    DrawGrid(10, 1.0f);
    EndMode3D();
    draw_statistics();
    EndDrawing();
}

void s_render::shutdown()
{
}

void s_render::draw_statistics()
{
    DrawText(("FPS: " + std::to_string(GetFPS())).c_str(), 50, 0, 50, {255, 0, 0, 255});
    DrawText(("DeltaTime: " + std::to_string(GetFrameTime())).c_str(), 50, 50, 50, {255, 0, 0, 255});
    DrawText(("Update: " + std::to_string(stats::timer_vector[stats::stat_type::UPDATE].count()) + " MS").c_str(), 50,
             100, 50, {255, 0, 0, 255});
    DrawText(("Render: " + std::to_string(stats::timer_vector[stats::stat_type::RENDER].count()) + " MS").c_str(), 50,
             150, 50, {255, 0, 0, 255});
    DrawText(("Benchmark: " + std::to_string(stats::timer_vector[stats::stat_type::BENCHMARK].count()) + " MS").c_str(),
             50, 200, 50, {255, 0, 0, 255});
}