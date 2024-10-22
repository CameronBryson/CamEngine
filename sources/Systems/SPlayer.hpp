#pragma once

#include "Graphics/Camera.hpp"
class registry;
class s_player
{
public:
    void update(registry& registry, Camera& camera, float dt);
    void shutdown();
private:
    void shoot();

};
