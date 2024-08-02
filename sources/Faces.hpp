#pragma once
#include "MathUtil.hpp"
#include "Vectors.hpp"
#include <vector>

class faces {
public:
    // Convert indices to triangles using the provided vertices
    static std::vector<std::vector<vec3>> get_triangles(const std::vector<vec3> &vertices, const std::vector<unsigned int> &indices) {
        std::vector<std::vector<vec3>> triangles;
        triangles.reserve(indices.size() / 3); // Reserve space for triangles
        for (size_t i = 0; i < indices.size(); i += 3) {
            triangles.emplace_back(std::initializer_list<vec3>{vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]});
        }
        return triangles;
    }

    // Get vertices for a quad with given extents
    static std::vector<vec3> get_quad_vertices(const vec3 &extents) {
        vec3 half_extents = extents * 0.5f;
        return {
            {-half_extents.x, -half_extents.y, -half_extents.z}, {half_extents.x, -half_extents.y, -half_extents.z},
            {half_extents.x, half_extents.y, -half_extents.z},   {-half_extents.x, half_extents.y, -half_extents.z},
            {-half_extents.x, -half_extents.y, half_extents.z},  {half_extents.x, -half_extents.y, half_extents.z},
            {half_extents.x, half_extents.y, half_extents.z},    {-half_extents.x, half_extents.y, half_extents.z}
        };
    }

    // Get indices for a quad
    static std::vector<unsigned int> get_quad_indices() {
        return {
            0, 1, 3, 3, 1, 2, 1, 5, 2, 2, 5, 6, 5, 4, 6, 6, 4, 7,
            4, 0, 7, 7, 0, 3, 3, 2, 7, 7, 2, 6, 4, 5, 0, 0, 5, 1
        };
    }

    // Get triangles for a quad with given extents
    static std::vector<std::vector<vec3>> get_quad_triangles(const vec3 &extents) {
        return get_triangles(get_quad_vertices(extents), get_quad_indices());
    }

    // Get vertices for a sphere with given radius, stacks, and sectors
    static std::vector<vec3> get_sphere_vertices(const float radius, const int stacks, const int sectors) {
        std::vector<vec3> vertices;
        vertices.reserve((stacks + 1) * (sectors + 1)); // Reserve space for vertices

        for (int i = 0; i <= stacks; ++i) {
            const float stack_angle = MathUtil::pi / 2 - i * MathUtil::pi / stacks;
            const float xy = radius * cosf(stack_angle);
            const float z = radius * sinf(stack_angle);

            for (int j = 0; j <= sectors; ++j) {
                const float sector_angle = j * 2 * MathUtil::pi / sectors;
                vertices.emplace_back(xy * cosf(sector_angle), xy * sinf(sector_angle), z);
            }
        }

        return vertices;
    }

    // Get indices for a sphere with given stacks and sectors
    static std::vector<unsigned int> get_sphere_indices(const int stacks, const int sectors) {
        std::vector<unsigned int> indices;
        indices.reserve(stacks * sectors * 6); // Reserve space for indices

        for (int i = 0; i < stacks; ++i) {
            for (int j = 0; j < sectors; ++j) {
                const int k1 = i * (sectors + 1) + j;
                const int k2 = k1 + sectors + 1;

                indices.emplace_back(k1);
                indices.emplace_back(k2);
                indices.emplace_back(k1 + 1);

                indices.emplace_back(k1 + 1);
                indices.emplace_back(k2);
                indices.emplace_back(k2 + 1);
            }
        }

        return indices;
    }

    // Get triangles for a sphere with given radius, stacks, and sectors
    static std::vector<std::vector<vec3>> get_sphere_triangles(const float radius, const int stacks, const int sectors) {
        return get_triangles(get_sphere_vertices(radius, stacks, sectors), get_sphere_indices(stacks, sectors));
    }
};