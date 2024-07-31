#include "SRender.hpp"

#include <string>

#include "Components.hpp"
#include "raylib.h"

#include "Stats.hpp"
#include "Faces.hpp"
#include "MathUtil.hpp"
void s_render::init()
{
    printf("Render init\n");
}

void s_render::update(const registry &registry, const camera &camera)
{
    const mat combined_matrix = camera.get_projection_matrix() * camera.get_view_matrix();
    std::vector<std::vector<vec3>> project_queue;
    std::vector<std::vector<vec2>> edge_queue;

    auto &positions = registry.get_sparse_set<c_transform>();

    auto &spheres = registry.get_sparse_set<c_sphere>();
    auto ids = registry.get_entity_ids<c_sphere>();

    for (const auto id : ids)
    {
        auto &[radius] = spheres.get_item(id);
        auto &[position, rotation, scale] = positions.get_item(id); // Assuming CTransform is also present
        mat matrix = mat::create_translation_matrix(position) * mat::create_rotation_matrix(rotation) * mat::create_scale_matrix(scale * 0.1f);
        for (auto &sphere_face : faces::get_sphere_faces(radius, 10, 10))
        {
            update_face(sphere_face, matrix);
            project_queue.push_back(sphere_face);
        }
    }

    auto &aabbs = registry.get_sparse_set<c_aabb>();
    ids = registry.get_entity_ids<c_aabb>();
    for (const auto id : ids)
    {
        auto &[extents] = aabbs.get_item(id);
        auto &[position, rotation, scale] = positions.get_item(id); // Assuming CTransform is also present
        mat matrix = mat::create_translation_matrix(position) * mat::create_rotation_matrix(rotation) * mat::create_scale_matrix(scale * 0.1f);
        for (auto &quad_face : faces::get_quad_faces(extents))
        {
            update_face(quad_face, matrix);
            project_queue.push_back(quad_face);
        }
    }
    for (const auto& face : project_queue)
    {
        for (size_t i = 0; i < face.size(); ++i)
        {
            const vec3 &start = face[i];
            const vec3 &end = face[(i + 1) % face.size()];
            vec2 projected_start = project(start, combined_matrix);
            vec2 projected_end = project(end, combined_matrix);
            edge_queue.push_back({projected_start, projected_end});
        }
    }
    BeginDrawing();
    ClearBackground(RAYWHITE);
    for (const auto &edge : edge_queue)
    {
        DrawLine(edge[0].x, edge[0].y, edge[1].x, edge[1].y, RED);
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
void s_render::update_face(std::vector<vec3> &face, const mat &matrix)
{
    for (auto &vertex : face)
    {
        vertex = matrix * vertex;
    }
}

vec2 s_render::project(const vec3& vertex, const mat& matrix)
{

    // Transform the vertex to clip space
    vec4 clip_space_vertex =  matrix * vec4(vertex.x, vertex.y, vertex.z, 1.0f);

    // Convert clip space coordinates to screen space coordinates
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    vec2 screen_space = vec2(
        (clip_space_vertex.x + 1.0f) * 0.5f * screen_width,
        (1.0f - clip_space_vertex.y) * 0.5f * screen_height
    );
    return screen_space;
}
