#include "SRender.hpp"
#include "Components.hpp"
#include "Faces.hpp"
#include "MathUtil.hpp"
#include "Stats.hpp"
#include "raylib.h"
#include <string>

void s_render::init()
{
    printf("Render init\n");
}

void s_render::update(const registry &registry, const camera &camera)
{
    const auto view_matrix = mat::create_view_matrix(camera.position, camera.target, camera.up);
    const auto projection_matrix =
        mat::create_perspective_matrix(camera.fov, camera.aspect_ratio, camera.near_plane, camera.far_plane);

    auto &positions = registry.get_sparse_set<c_transform>();
    auto &spheres = registry.get_sparse_set<c_sphere>();
    auto &aabbs = registry.get_sparse_set<c_aabb>();
    std::deque<std::vector<vec3>> triangle_queue;

    for (const auto id : registry.get_entity_ids<c_sphere>())
    {
        auto &[radius] = spheres.get_item(id);
        auto &[position, rotation, scale] = positions.get_item(id);
        mat model_matrix = mat::create_translation_matrix(position) * mat::create_rotation_matrix(rotation) *
                           mat::create_scale_matrix(scale);
        for (auto &sphere_triangle : faces::get_sphere_triangles(radius, 10, 10))
        {
            update_triangle(sphere_triangle, model_matrix);
            for (auto &vertex : sphere_triangle)
            {
                project(vertex, view_matrix, projection_matrix);
            }
            if (is_triangle_visable(sphere_triangle, camera))
                triangle_queue.push_back(sphere_triangle);
        }
    }

    for (const auto id : registry.get_entity_ids<c_aabb>())
    {
        auto &[extents] = aabbs.get_item(id);
        auto &[position, rotation, scale] = positions.get_item(id);
        mat model_matrix = mat::create_translation_matrix(position) * mat::create_rotation_matrix(rotation) *
                           mat::create_scale_matrix(scale);
        for (auto &quad_triangle : faces::get_quad_triangles(extents))
        {
            update_triangle(quad_triangle, model_matrix);
            for (auto &vertex : quad_triangle)
            {
                project(vertex, view_matrix, projection_matrix);
            }
            if (is_triangle_visable(quad_triangle, camera))
                triangle_queue.push_back(quad_triangle);
        }
    }


    //project_triangles(triangle_queue, view_matrix, projection_matrix);
    draw_triangles(triangle_queue);
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

void s_render::update_triangle(std::vector<vec3> &triangle, const mat &local_matrix)
{
    for (auto &vertex : triangle)
    {
        vertex = local_matrix * vertex;
    }
}

void s_render::draw_triangles(const std::deque<std::vector<vec3>> &draw_queue)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    for (const auto &triangle : draw_queue)
    {
        for (int i = 0; i < triangle.size(); ++i)
        {
            const vec3 start = triangle[i];
            const vec3 end = triangle[(i + 1) % triangle.size()];
            DrawLine(start.x, start.y, end.x, end.y, RED);
        }
    }
    draw_statistics();
    EndDrawing();
}

void s_render::project_triangles(std::deque<std::vector<vec3>> &triangle_queue, const mat &view_matrix,
                                 const mat &projection_matrix)
{
    for (auto &triangle : triangle_queue)
    {
        for (auto &vertex : triangle)
        {
            project(vertex, view_matrix, projection_matrix);
        }
    }
}
bool s_render::is_triangle_visable(const std::vector<vec3> &triangle, const camera &camera)
{
    auto ab = triangle[1] - triangle[0];
    auto ac = triangle[2] - triangle[0];
    auto sign = ab.x * ac.y - ac.x * ab.y;
    return sign < 0;
}

// Update the project function in `sources/SRender.cpp`
void s_render::project(vec3 &vertex, const mat &view_matrix, const mat &projection_matrix)
{
    // auto tempVertex = glm::vec4(vertex.x, vertex.y, vertex.z,1.0f);
    auto tempVertex = vec4(vertex.x, vertex.y, vertex.z, 1.0f);
    tempVertex = view_matrix * tempVertex;
    tempVertex = projection_matrix * tempVertex;
    tempVertex /= tempVertex.w;
    // Convert the normalized device coordinates to screen space coordinates
    vertex.x = (tempVertex.x + 1.0f) * 0.5f * GetScreenWidth();
    vertex.y = (1.0f - tempVertex.y) * 0.5f * GetScreenHeight();
}