#pragma once
#include "Math/Vectors.hpp"
#include "Math/Matrix.hpp"
#include "Math/MathUtil.hpp"

class camera {
public:
    camera() : position(0.0f, 0.0f, 5.0f), target(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f), fov(90.0f), aspect_ratio(16.0f / 9.0f), near_plane(0.1f), far_plane(100.0f) {
        update_view_matrix();
        update_projection_matrix();
    }

    vec3 position;
    vec3 target;
    vec3 up;
    float fov;
    float aspect_ratio;
    float near_plane;
    float far_plane;
    mat4 view_matrix;
    mat4 projection_matrix;
    void update_view_matrix() {
        view_matrix = mat4::create_view_matrix(position, target, up);
    }
    void update_projection_matrix() {
        projection_matrix = mat4::create_perspective_matrix(fov, aspect_ratio, near_plane, far_plane);
    }
};