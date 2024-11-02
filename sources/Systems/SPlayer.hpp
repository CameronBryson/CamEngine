#pragma once

#include "Graphics/Camera.hpp"
class Registry;
class SPlayer
{
public:
    void update(Registry& registry, Camera& camera, float dt);
    void shutdown();
private:
    void shoot();

};
