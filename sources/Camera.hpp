#pragma once
#include "MathUtil.hpp"
#include "Vectors.hpp"
#include "Matrix.hpp"

class camera {
public:
    camera() : position(0.0f, 0.0f, 5.0f), target(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f), fov(90.0f), aspect_ratio(16.0f / 9.0f), near_plane(0.1f), far_plane(100.0f) {
    }

    vec3 position;
    vec3 target;
    vec3 up;
    float fov;
    float aspect_ratio;
    float near_plane;
    float far_plane;
};