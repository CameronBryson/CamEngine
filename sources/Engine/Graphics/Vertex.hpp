#pragma once
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

struct Vertex
{
    Vertex() = default;
    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex)
        : position(pos), normal(norm), texture_coordinates(tex)
    {
    }

    glm::vec3 position = { 0, 0, 0 };
    glm::vec3 normal = { 0, 0, 0 };
    glm::vec2 texture_coordinates = { 0, 0 };

    bool operator==(const Vertex& other) const
    {
        return position == other.position &&
            normal == other.normal &&
            texture_coordinates == other.texture_coordinates;
    }
};
// Hash Function for Vertex
namespace std
{
    template<> struct hash<Vertex>
    {
        size_t operator()(const Vertex& vertex) const
        {
            size_t seed = 0;
            // Hashing position
            for (int i = 0; i < 3; ++i)
            {
                seed ^= std::hash<float>()(vertex.position[i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            // Hashing normal
            for (int i = 0; i < 3; ++i)
            {
                seed ^= std::hash<float>()(vertex.normal[i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            // Hashing texture coordinates
            for (int i = 0; i < 2; ++i)
            {
                seed ^= std::hash<float>()(vertex.texture_coordinates[i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
}