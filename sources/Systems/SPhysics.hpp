#pragma once
class Registry;

class SPhysics
{
public:
    void update(const Registry& registry, float dt);
    void shutdown();
private:
    void updateDynamicBodies(const Registry& registry, float dt);
};
