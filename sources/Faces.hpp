#pragma once
#include <vector>
#include "Vectors.hpp"

class faces {
public:
    static std::vector<std::vector<vec3>> get_quad_faces(const vec3& extents) {
        vec3 half_extents = extents * 0.5f;

        std::vector<vec3> vertices = {
            { -half_extents.x, -half_extents.y, -half_extents.z },
            {  half_extents.x, -half_extents.y, -half_extents.z },
            {  half_extents.x,  half_extents.y, -half_extents.z },
            { -half_extents.x,  half_extents.y, -half_extents.z },
            { -half_extents.x, -half_extents.y,  half_extents.z },
            {  half_extents.x, -half_extents.y,  half_extents.z },
            {  half_extents.x,  half_extents.y,  half_extents.z },
            { -half_extents.x,  half_extents.y,  half_extents.z }
        };

        return {
                { vertices[0], vertices[1], vertices[2], vertices[3] }, // Front
                { vertices[4], vertices[5], vertices[6], vertices[7] }, // Back
                { vertices[0], vertices[3], vertices[7], vertices[4] }, // Left
                { vertices[1], vertices[2], vertices[6], vertices[5] }, // Right
                { vertices[3], vertices[2], vertices[6], vertices[7] }, // Top
                { vertices[0], vertices[1], vertices[5], vertices[4] }  // Bottom
        };
    }
    static std::vector<std::vector<vec3>> get_sphere_faces(const float radius, const int stacks, const int sectors)
    {
        std::vector<vec3> vertices;
        std::vector<std::vector<vec3>> faces;

        vertices.reserve((stacks + 1) * (sectors + 1));
        faces.reserve(stacks * sectors * 2);

        // Generate vertices
        for (int i = 0; i <= stacks; ++i)
        {
            const float stack_angle = MathUtil::pi / 2 - i * MathUtil::pi / stacks;
            const float xy = radius * cosf(stack_angle);
            float z = radius * sinf(stack_angle);

            for (int j = 0; j <= sectors; ++j)
            {
                const float sector_angle = j * 2 * MathUtil::pi / sectors;
                vertices.push_back({xy * cosf(sector_angle), xy * sinf(sector_angle), z});
            }
        }

        // Generate faces
        for (int i = 0; i < stacks; ++i)
        {
            int k1 = i * (sectors + 1);
            int k2 = k1 + sectors + 1;

            for (int j = 0; j < sectors; ++j, ++k1, ++k2)
            {
                if (i != 0)
                    faces.push_back({vertices[k1], vertices[k2], vertices[k1 + 1]});
                if (i != (stacks - 1))
                    faces.push_back({vertices[k1 + 1], vertices[k2], vertices[k2 + 1]});
            }
        }

        return faces;
    }
};