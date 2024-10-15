#pragma once
class registry;

class s_physics
{
public:
    void update(const registry& registry, float dt);
    void shutdown();
private:
    void update_dynamic_bodies(const registry& registry, float dt);
};
