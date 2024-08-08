#pragma once
#include "Engine/Registry.hpp"
#include "Graphics/Camera.hpp"
#include "platform.hpp"

#include <Graphics/GraphicsManager.hpp>
class s_render
{
  public:
    static void init();
    static void update(const registry &registry, Camera &camera);
    static void shutdown();

  private:
    static void draw_statistics();
    static void load_shaders();
    static void load_textures();
};
