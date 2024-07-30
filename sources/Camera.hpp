//
// Created by cam on 29/07/24.
//

#ifndef CAMERA_HPP
#define CAMERA_HPP
#include <Vectors.hpp>

struct camera
{
    vec3 position;
    vec3 target;
    vec3 up;
    float fov;
};

#endif // CAMERA_HPP
