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
};