#include "SRender.hpp"

#include <string>

#include "Components.hpp"
#include "raylib.h"

#include "Stats.hpp"
#include "Faces.hpp"
#include "Quaternion.hpp"
#include "MathUtil.hpp"
void s_render::init()
{
    printf("Render init\n");
}

void s_render::update(const registry &registry, const camera &camera)
{

    std::vector<std::vector<vec3>> draw_queue;
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
        //sphere stuff here
        for (auto &sphere_face : faces::get_sphere_faces(radius,10,10))
        {
            update_face(sphere_face, scale, rotation, position);
            draw_queue.push_back(sphere_face);
        }
    }

    // Draw CAABB
    auto &aabbs = registry.get_sparse_set<c_aabb>();
    ids = registry.get_entity_ids<c_aabb>();
    for (const auto id : ids)
    {
        auto &[extents] = aabbs.get_item(id);
        auto &[position, rotation, scale] = positions.get_item(id); // Assuming CTransform is also present
        for (auto &quad_face : faces::get_quad_faces(extents))
        {
            update_face(quad_face, scale, rotation, position);
            draw_queue.push_back(quad_face);
        }

    }
    BeginDrawing();
    ClearBackground(RAYWHITE);
    for (const auto &face : draw_queue)
    {
        draw_face(face, camera);
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
void s_render::draw_face(const std::vector<vec3> &face, const camera& camera)
{
    for (size_t i = 0; i < face.size(); ++i)
    {
        const auto start = face[i];
        const auto end = face[(i + 1) % face.size()];
        draw_edge({start.x, start.y, start.z}, {end.x, end.y, end.z}, camera);
    }
}
void s_render::draw_edge(const vec3 &start, const vec3 &end, const camera& camera)
{
    const vec2 start_projected = project(start, camera);
    const vec2 end_projected = project(end, camera);
    DrawLine(start_projected.x, start_projected.y, end_projected.x, end_projected.y, RED);
}
vec2 s_render::project(const vec3 &vertex, const camera &camera)
{
    vec3 camera_direction = (camera.target - camera.position).normalized();
    vec3 camera_right = MathUtil::cross_product(camera.up, camera_direction).normalized();
    vec3 camera_up = MathUtil::cross_product(camera_direction, camera_right);

    vec3 relative_vertex = vertex - camera.position;
    vec3 transformed_vertex = {
        MathUtil::dot_product(relative_vertex, camera_right),
        MathUtil::dot_product(relative_vertex, camera_up),
        MathUtil::dot_product(relative_vertex, camera_direction)
    };

    float aspect_ratio = static_cast<float>(GetScreenWidth()) / GetScreenHeight();
    vec2 projected_vertex = {
        transformed_vertex.x / transformed_vertex.z,
        (transformed_vertex.y / transformed_vertex.z) * aspect_ratio
    };

    projected_vertex.x = (projected_vertex.x + 1.0f) * 0.5f * GetScreenWidth();
    projected_vertex.y = (1.0f - projected_vertex.y) * 0.5f * GetScreenHeight();

    return projected_vertex;
}
void s_render::update_face(std::vector<vec3> &face, const vec3 &scale, const quat &rotation, const vec3 &translation)
{
    for (auto &vertex : face)
    {
        vertex *= scale;
        vertex = rotation * vertex;
        vertex += translation;
    }
}
