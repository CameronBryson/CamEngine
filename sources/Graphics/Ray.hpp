#pragma once
#include "../Engine/Vectors.hpp"
class ray
{
public:
    glm::vec3 position;
    glm::vec3 direction;
    ray(const glm::vec3 &position, const glm::vec3 &direction) : position(position), direction(direction.normalized()) {}
    [[nodiscard]] glm::vec3 get_point(const float distance) const
    {
        return position + direction * distance;
    }
};

