#pragma once
#include "Engine/Registry.hpp"

class s_player
{
  public:
    static void update(const registry &registry, float dt);
    static void shutdown();
};
