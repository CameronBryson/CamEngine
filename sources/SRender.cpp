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
    const auto view_matrix = mat4::create_view_matrix(camera.position, camera.target, camera.up);
    const auto projection_matrix =
        mat4::create_perspective_matrix(camera.fov, camera.aspect_ratio, camera.near_plane, camera.far_plane);

    auto &positions = registry.get_sparse_set<c_transform>();
    auto &spheres = registry.get_sparse_set<c_sphere>();
    auto &aabbs = registry.get_sparse_set<c_quad>();
    std::deque<std::vector<vec3>> triangle_queue;

    for (const auto id : registry.get_entity_ids<c_sphere>())
    {
        auto &[radius] = spheres.get_item(id);
        auto &[position, rotation, scale] = positions.get_item(id);
        mat4 model_matrix = mat4::create_translation_matrix(position) * mat4::create_rotation_matrix(rotation) *
                           mat4::create_scale_matrix(scale);
        for (auto &sphere_triangle : faces::get_sphere_triangles(radius, 5, 9))
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

    for (const auto id : registry.get_entity_ids<c_quad>())
    {
        auto &[extents] = aabbs.get_item(id);
        auto &[position, rotation, scale] = positions.get_item(id);
        mat4 model_matrix = mat4::create_translation_matrix(position) * mat4::create_rotation_matrix(rotation) *
                           mat4::create_scale_matrix(scale);
        for (auto &quad_triangle : faces::get_quad_triangles(extents))
        {
            update_triangle(quad_triangle, model_matrix);
            for (auto &vertex : quad_triangle)
            {
                project(vertex, view_matrix, projection_matrix);
            }
            if (!is_triangle_visable(quad_triangle, camera))
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
    const std::string stats_text = "FPS: " + std::to_string(GetFPS()) + "\n" +
                             "DeltaTime: " + std::to_string(GetFrameTime()) + "\n" +
                             "Update: " + std::to_string(stats::timer_vector[stats::stat_type::UPDATE].count()) + " MS\n" +
                             "Render: " + std::to_string(stats::timer_vector[stats::stat_type::RENDER].count()) + " MS\n" +
                             "Benchmark: " + std::to_string(stats::timer_vector[stats::stat_type::BENCHMARK].count()) + " MS";
    DrawText(stats_text.c_str(), 50, 0, 50, {255, 0, 0, 255});
}

void s_render::update_triangle(std::vector<vec3> &triangle, const mat4 &local_matrix)
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

void s_render::project_triangles(std::deque<std::vector<vec3>> &triangle_queue, const mat4 &view_matrix,
                                 const mat4 &projection_matrix)
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
    vec3 ab = triangle[1] - triangle[0];
    vec3 ac = triangle[2] - triangle[0];
    float sign = ab.x * ac.y - ac.x * ab.y;
    return sign >  0;
}

// Update the project function in `sources/SRender.cpp`
void s_render::project(vec3 &vertex, const mat4 &view_matrix, const mat4 &projection_matrix)
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