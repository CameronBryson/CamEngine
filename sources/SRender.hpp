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
  static vec2 project(const vec3& vertex, const mat& matrix);
  static void update_face(std::vector<vec3> &face, const mat& matrix);
};
