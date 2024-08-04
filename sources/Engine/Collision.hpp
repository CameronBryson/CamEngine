#pragma once
#include "Components.hpp"
#include "Math/Vectors.hpp"

class collision_manifold {
public:
    collision_manifold(const vec3 normal, const float penetration_depth, c_transform &transform1, c_transform &transform2, object_collision_type type1, object_collision_type type2)
        : normal(normal), penetration_depth_(penetration_depth), transform1_(transform1), transform2_(transform2), type1(type1), type2(type2) {}

    void resolve_collision() const {
        if (type1 == object_collision_type::DYNAMIC && type2 == object_collision_type::DYNAMIC) {
            resolve_dynamic_vs_dynamic();
        } else if (type1 == object_collision_type::DYNAMIC && (type2 == object_collision_type::STATIC || type2 == object_collision_type::KINEMATIC)) {
            resolve_dynamic_vs_not_dynamic(transform1_);
        } else if (type2 == object_collision_type::DYNAMIC && (type1 == object_collision_type::STATIC || type1 == object_collision_type::KINEMATIC)) {
            resolve_dynamic_vs_not_dynamic(transform2_);
        } else if (type1 == object_collision_type::KINEMATIC && type2 == object_collision_type::KINEMATIC) {
            resolve_kinematic_vs_kinematic();
        } else if (type1 == object_collision_type::KINEMATIC && type2 == object_collision_type::STATIC) {
            resolve_kinematic_vs_static(transform1_);
        } else if (type2 == object_collision_type::KINEMATIC && type1 == object_collision_type::STATIC) {
            resolve_kinematic_vs_static(transform2_);
        } else if (type1 == object_collision_type::STATIC && type2 == object_collision_type::STATIC) {
            // No movement for static vs static
        }
    }

    float penetration_depth_ = 0;
    vec3 normal;

private:
    void resolve_dynamic_vs_dynamic() const {
        transform1_.position += normal * penetration_depth_ * 0.5f;
        transform2_.position -= normal * penetration_depth_ * 0.5f;
    }

    void resolve_dynamic_vs_not_dynamic(c_transform &dynamic_transform) const {
        dynamic_transform.position -= normal * penetration_depth_;
    }

    void resolve_kinematic_vs_kinematic() const {
        transform1_.position += normal * penetration_depth_ * 0.5f;
        transform2_.position -= normal * penetration_depth_ * 0.5f;
    }

    void resolve_kinematic_vs_static(c_transform &kinematic_transform) const {
        kinematic_transform.position += normal * penetration_depth_;
    }

    object_collision_type type1;
    object_collision_type type2;
    c_transform &transform1_;
    c_transform &transform2_;
};