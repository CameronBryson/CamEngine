#pragma once
#include "Engine/Registry.hpp"

class s_player
{
public:
    static void update(registry& registry, float dt);
    static void shutdown();
};
