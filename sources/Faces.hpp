
#pragma once
#include <Vectors.hpp>
#include <vector>

class faces {
public:
    static std::vector<std::vector<vec3>> get_quad_faces(const vec3& position, const vec3& extents)
    {
        const vec3 half_extents = extents * 0.5f;
        const vec3 min = position - half_extents;
        const vec3 max = position + half_extents;

        return {
            // Front
            {
                {min.x, min.y, min.z},
                {max.x, min.y, min.z},
                {max.x, max.y, min.z},
                {min.x, max.y, min.z}
            },
            // Back
            {
                {min.x, min.y, max.z},
                {max.x, min.y, max.z},
                {max.x, max.y, max.z},
                {min.x, max.y, max.z}
            },
            // Left
            {
                {min.x, min.y, min.z},
                {min.x, min.y, max.z},
                {min.x, max.y, max.z},
                {min.x, max.y, min.z}
            },
            // Right
            {
                {max.x, min.y, min.z},
                {max.x, min.y, max.z},
                {max.x, max.y, max.z},
                {max.x, max.y, min.z}
            },
            // Top
            {
                {min.x, max.y, min.z},
                {max.x, max.y, min.z},
                {max.x, max.y, max.z},
                {min.x, max.y, max.z}
            },
            // Bottom
            {
                {min.x, min.y, min.z},
                {max.x, min.y, min.z},
                {max.x, min.y, max.z},
                {min.x, min.y, max.z}
            }
        };
    }
};