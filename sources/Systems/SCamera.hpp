#pragma once
#include "Graphics/Camera.hpp"

#include "Engine/Registry.hpp"

class s_camera
{
public:
    static void update(Camera& camera, float dt);
};