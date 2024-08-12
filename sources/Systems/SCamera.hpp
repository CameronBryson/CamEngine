//
// Created by cam on 30/07/24.
//

#ifndef SCAMERA_HPP
#define SCAMERA_HPP
#include "Graphics/Camera.hpp"

#include "Engine/Registry.hpp"

class s_camera
{
public:
    static void update(Camera& camera, float dt);
};


#endif //SCAMERA_HPP
