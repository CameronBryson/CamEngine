#pragma once
#include "Vectors.hpp"

#include <Components.hpp>
#include <vector>
class collision_manifold
{
public:
    collision_manifold(const vec3 normal, const float penetration_depth, c_transform &transform1, c_transform &transform2)
        : normal(normal), penetration_depth_(penetration_depth), transform1_(transform1), transform2_(transform2)
    {
    }
    void resolve_collision() const
    {
        //vec3 correction = normal_ * (penetration_depth_*0.5f);
        transform1_.position += normal * penetration_depth_/2;
        transform2_.position -= normal * penetration_depth_/2;
    }
    float penetration_depth_ = 0;
    vec3 normal;
private:
    c_transform &transform1_;
    c_transform &transform2_;
};
