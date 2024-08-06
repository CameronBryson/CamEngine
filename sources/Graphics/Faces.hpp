#pragma once
#include <vector>

class faces
{
  public:
    // Get vertices for a quad with given extents
    static std::vector<float> get_quad_vertices()
    {
        return {-0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f,
                -0.5f, -0.5f, 0.5f,  0.5f, -0.5f, 0.5f,  0.5f, 0.5f, 0.5f,  -0.5f, 0.5f, 0.5f};
    }

    // Get indices for a quad
    static std::vector<unsigned int> get_quad_indices()
    {
        return {
            0, 1, 2, 2, 3, 0, // Front face
            4, 5, 6, 6, 7, 4, // Back face
            0, 1, 5, 5, 4, 0, // Bottom face
            2, 3, 7, 7, 6, 2, // Top face
            0, 3, 7, 7, 4, 0, // Left face
            1, 2, 6, 6, 5, 1  // Right face
        };
    }

    // Get vertices for a sphere with given radius, stacks, and sectors
    static std::vector<float> get_sphere_vertices(const int stacks, const int sectors)
    {
        std::vector<float> vertices;
        const float PI = 3.14159265359f;
        for (int i = 0; i <= stacks; ++i)
        {
            float stackAngle = PI / 2 - i * PI / stacks;
            float xy = cosf(stackAngle);
            float z = sinf(stackAngle);

            for (int j = 0; j <= sectors; ++j)
            {
                float sectorAngle = j * 2 * PI / sectors;
                float x = xy * cosf(sectorAngle);
                float y = xy * sinf(sectorAngle);
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
        }
        return vertices;
    }

    // Get indices for a sphere with given stacks and sectors
    static std::vector<unsigned int> get_sphere_indices(const int stacks, const int sectors)
    {
        std::vector<unsigned int> indices;
        for (int i = 0; i < stacks; ++i)
        {
            for (int j = 0; j < sectors; ++j)
            {
                int first = (i * (sectors + 1)) + j;
                int second = first + sectors + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }
        return indices;
    }
};