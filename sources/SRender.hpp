#pragma once
#include "Camera.hpp"
#include "raylib.h"

#include <Registry.hpp>
class s_render
{
  public:
    static void init();
    static void update(const registry &registry, const camera &camera);
    static void shutdown();

  private:
    static void draw_statistics();
    static void project(vec3 &vertex, const mat4& view_matrix, const mat4& projection_matrix);
    static void update_triangle(std::vector<vec3> &triangle, const mat4& local_matrix);
    static void draw_triangles(const std::deque<std::vector<vec3>> &triangle_queue);
    static void project_triangles(std::deque<std::vector<vec3>> &triangle_queue,const mat4& view_matrix, const mat4& projection_matrix);
    static bool is_triangle_visable(const std::vector<vec3> &triangle,const camera& camera);
};
