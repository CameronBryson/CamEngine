#pragma once
#include "Camera.hpp"
#include "raylib.h"

#include <Quaternion.hpp>
#include <Registry.hpp>

class s_render
{
  public:
    static void init();
    static void update(const registry &registry, const camera &camera);
    static void shutdown();

  private:
    static void draw_statistics();
    static void draw_face(const std::vector<vec3> &face, const camera& camera);
    static void draw_edge(const vec3 &start, const vec3 &end, const camera& camera);
    static vec2 project(const vec3& vertex, const camera& camera);
    static void update_face(std::vector<vec3> &face, const vec3& scale, const quat& rotation, const vec3& translation);
};
