#pragma once
#include "Vectors.hpp"
#include "MathUtil.hpp"
class faces {
public:
    static std::vector<vec3> get_quad_verticies(const vec3& extents)
    {
        vec3 half_extents = extents * 0.5f;
        return{
                { -half_extents.x, -half_extents.y, -half_extents.z },
                {  half_extents.x, -half_extents.y, -half_extents.z },
                {  half_extents.x,  half_extents.y, -half_extents.z },
                { -half_extents.x,  half_extents.y, -half_extents.z },
                { -half_extents.x, -half_extents.y,  half_extents.z },
                {  half_extents.x, -half_extents.y,  half_extents.z },
                {  half_extents.x,  half_extents.y,  half_extents.z },
                { -half_extents.x,  half_extents.y,  half_extents.z }
        };
    }

    static std::vector<std::vector<vec3>> get_quad_triangles(const vec3& extents) {

        auto vertices = get_quad_verticies(extents);

        return {
            // Front face (counterclockwise)
                { vertices[0], vertices[1], vertices[2] },
                { vertices[2], vertices[3], vertices[0] },
                // Back face (counterclockwise)
                { vertices[5], vertices[4], vertices[7] },
                { vertices[7], vertices[6], vertices[5] },
                // Left face (counterclockwise)
                { vertices[4], vertices[0], vertices[3] },
                { vertices[3], vertices[7], vertices[4] },
                // Right face (counterclockwise)
                { vertices[1], vertices[5], vertices[6] },
                { vertices[6], vertices[2], vertices[1] },
                // Top face (counterclockwise)
                { vertices[3], vertices[2], vertices[6] },
                { vertices[6], vertices[7], vertices[3] },
                // Bottom face (counterclockwise)
                { vertices[4], vertices[5], vertices[1] },
                { vertices[1], vertices[0], vertices[4] }
        };
    }



    static std::vector<std::vector<vec3>> get_sphere_triangles(const float radius, const int stacks, const int sectors)
    {
        std::vector<vec3> vertices;
        std::vector<std::vector<vec3>> triangles;

        vertices.reserve((stacks + 1) * (sectors + 1));
        triangles.reserve(stacks * sectors * 2);

        // Generate vertices
        for (int i = 0; i <= stacks; ++i)
        {
            const float stack_angle = MathUtil::pi / 2 - i * MathUtil::pi / stacks;
            const float xy = radius * cosf(stack_angle);
            const float z = radius * sinf(stack_angle);

            for (int j = 0; j <= sectors; ++j)
            {
                const float sector_angle = j * 2 * MathUtil::pi / sectors;
                vertices.push_back({xy * cosf(sector_angle), xy * sinf(sector_angle), z});
            }
        }

        // Generate triangles
        for (int i = 0; i < stacks; ++i)
        {
            int k1 = i * (sectors + 1);
            int k2 = k1 + sectors + 1;

            for (int j = 0; j < sectors; ++j, ++k1, ++k2)
            {
                if (i != 0)
                    triangles.push_back({vertices[k1], vertices[k2], vertices[k1 + 1]});
                if (i != (stacks - 1))
                    triangles.push_back({vertices[k1 + 1], vertices[k2], vertices[k2 + 1]});
            }
        }

        return triangles;
    }
};