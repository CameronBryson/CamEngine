#pragma once
#include "../Engine/Vectors.hpp"
class ray
{
public:
    vec3 position;
    vec3 direction;
    ray(const vec3 &position, const vec3 &direction) : position(position), direction(direction.normalized()) {}
    [[nodiscard]] vec3 get_point(const float distance) const
    {
        return position + direction * distance;
    }
};

