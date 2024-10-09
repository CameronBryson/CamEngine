#pragma once
class registry;

class s_physics
{
public:
    static void update(const registry& registry, float dt);
    static void update_dynamic_bodies(const registry& registry, float dt);
    static void shutdown();
};
