#include "SRender.hpp"

#include <string>

#include "Components.hpp"
#include "raylib.h"

#include "Stats.hpp"
#include "Faces.hpp"

void s_render::init()
{
    printf("Render init\n");
}

void s_render::update(const registry &registry, const camera &camera)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

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
        DrawSphereWires({position.x, position.y, position.z}, radius, 16, 16, DARKBLUE);
    }

    // Draw CAABB
    auto &aabbs = registry.get_sparse_set<c_aabb>();
    ids = registry.get_entity_ids<c_aabb>();
    for (const auto id : ids)
    {
        auto &[extents] = aabbs.get_item(id);
        auto &[position, rotation, scale] = positions.get_item(id); // Assuming CTransform is also present
        //DrawCube({position.x, position.y, position.z}, extents.x, extents.y, extents.z, GREEN);
        //DrawCubeWires({position.x, position.y, position.z}, extents.x, extents.y, extents.z, DARKGREEN);
        for (const auto &face : faces::get_quad_faces(position, extents))
        {
            draw_face(face, position);
        }

    }
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
void s_render::draw_face(const std::vector<vec3> &face, const vec3 &position)
{
    for (size_t i = 0; i < face.size(); ++i)
    {
        const auto start = face[i];
        const auto end = face[(i + 1) % face.size()];
        draw_edge({start.x, start.y, start.z}, {end.x, end.y, end.z});
    }
}
void s_render::draw_edge(const vec3 &start, const vec3 &end)
{
    const vec2 start_projected = project(start, 10.0f);
    const vec2 end_projected = project(end, 10.0f);
    DrawLine(start_projected.x, start_projected.y, end_projected.x, end_projected.y, RED);
}
vec2 s_render::project(const vec3 &vertex, const float focal_length)
{
    vec2 projected_vertex;
    constexpr float scale_factor = 10.0f;

    // Check for division by zero
    if (vertex.z != 0.0f)
    {
        projected_vertex.x = (vertex.x * focal_length / vertex.z) * scale_factor;
        projected_vertex.y = (vertex.y * focal_length / vertex.z) * scale_factor;
    }
    else
    {
        // Handle the case where z is zero
        projected_vertex.x = vertex.x * scale_factor;
        projected_vertex.y = vertex.y * scale_factor;
    }

    return projected_vertex;
}
