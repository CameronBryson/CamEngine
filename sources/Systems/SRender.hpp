#pragma once
#include "Graphics/Camera.hpp"
#include "Engine/Registry.hpp"
#include "platform.hpp"
class s_render
{
  public:
    static void init();
    static void update(const registry &registry, const camera &camera);
    static void shutdown();

  private:
    static void draw_statistics();
};
