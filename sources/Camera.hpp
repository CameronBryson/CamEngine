#pragma once
#include "MathUtil.hpp"
#include "Vectors.hpp"
#include "Matrix.hpp"

class camera {
public:
    camera() : position(1.0f, 1.0f, 1.0f), target(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f), fov(90.0f), aspect_ratio(16.0f / 9.0f), near_plane(0.1f), far_plane(100.0f) {
        update_view_matrix();
        update_projection_matrix();
    }

    vec3 position;
    vec3 target;
    vec3 up;

    void update_view_matrix() {
        vec3 zaxis = (position - target).normalized();
        vec3 xaxis = MathUtil::cross_product(up, zaxis).normalized();
        vec3 yaxis = MathUtil::cross_product(zaxis, xaxis);

        view_matrix_ = mat({
            xaxis.x, yaxis.x, zaxis.x, 0,
            xaxis.y, yaxis.y, zaxis.y, 0,
            xaxis.z, yaxis.z, zaxis.z, 0,
            -MathUtil::dot_product(xaxis, position), -MathUtil::dot_product(yaxis, position), -MathUtil::dot_product(zaxis, position), 1
        });
    }

    void update_projection_matrix() {
        float tan_half_fov = std::tan(MathUtil::to_radians(fov) / 2.0f);
        projection_matrix_ = mat({
            1.0f / (aspect_ratio * tan_half_fov), 0, 0, 0,
            0, 1.0f / tan_half_fov, 0, 0,
            0, 0, -(far_plane + near_plane) / (far_plane - near_plane), -1,
            0, 0, -(2.0f * far_plane * near_plane) / (far_plane - near_plane), 0
        });
    }

    mat get_view_matrix() const {
        return view_matrix_;
    }

    mat get_projection_matrix() const {
        return projection_matrix_;
    }

private:
    float fov;
    float aspect_ratio;
    float near_plane;
    float far_plane;
    mat view_matrix_;
    mat projection_matrix_;
};