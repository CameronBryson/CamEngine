#pragma once
#include "MathUtil.hpp"
#include "Vectors.hpp"
#include "Matrix.hpp"

class camera {
public:
    camera() : position(1.0f, 1.0f, 1.0f), target(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f), fov_(90.0f), aspect_ratio_(16.0f / 9.0f), near_plane_(0.1f), far_plane_(100.0f) {
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
        const float tan_half_fov = std::tan(MathUtil::to_radians(fov_) / 2.0f);
        projection_matrix_ = mat({
            1.0f / (aspect_ratio_ * tan_half_fov), 0, 0, 0,
            0, 1.0f / tan_half_fov, 0, 0,
            0, 0, -(far_plane_ + near_plane_) / (far_plane_ - near_plane_), -1,
            0, 0, -(2.0f * far_plane_ * near_plane_) / (far_plane_ - near_plane_), 0
        });
    }

    mat get_view_matrix() const {
        return view_matrix_;
    }

    mat get_projection_matrix() const {
        return projection_matrix_;
    }

private:
    float fov_;
    float aspect_ratio_;
    float near_plane_;
    float far_plane_;
    mat view_matrix_;
    mat projection_matrix_;
};