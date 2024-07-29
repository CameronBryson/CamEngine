#pragma once
#include "raylib.h"
#include <Registry.hpp>

class s_render
{
  public:
    static void init();
    static void update(const registry &registry, const Camera3D &camera);
    static void shutdown();

  private:
    static void draw_statistics();
};
