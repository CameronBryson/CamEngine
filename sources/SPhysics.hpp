#pragma once
#include <Registry.hpp>
class s_physics
{
  public:
    static void update(const registry &registry, float dt);
    static void shutdown();
};
