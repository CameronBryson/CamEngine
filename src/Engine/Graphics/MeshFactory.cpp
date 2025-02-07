#include "pch.hpp"
#include "MeshFactory.hpp"
#include "Material.hpp"
#include <glm/glm.hpp>

std::shared_ptr<Mesh> MeshFactory::createCube()
{
    // Define the vertices and indices for a cube
    std::vector<Vertex> vertices = {
        // Positions          // Normals         // TexCoords
        {{-1.0f, -1.0f, -1.0f}, {}, {}},
        {{ 1.0f, -1.0f, -1.0f}, {}, {}},
        {{ 1.0f,  1.0f, -1.0f}, {}, {}},
        {{-1.0f,  1.0f, -1.0f}, {}, {}},
        {{-1.0f, -1.0f,  1.0f}, {}, {}},
        {{ 1.0f, -1.0f,  1.0f}, {}, {}},
        {{ 1.0f,  1.0f,  1.0f}, {}, {}},
        {{-1.0f,  1.0f,  1.0f}, {}, {}},
    };

    std::vector<unsigned int> indices = {
        // Back face
        0, 1, 2, 2, 3, 0,
        // Front face
        4, 5, 6, 6, 7, 4,
        // Left face
        4, 0, 3, 3, 7, 4,
        // Right face
        1, 5, 6, 6, 2, 1,
        // Bottom face
        4, 5, 1, 1, 0, 4,
        // Top face
        3, 2, 6, 6, 7, 3
    };


    // Create and return the mesh
    return Mesh::createMesh(vertices, indices, nullptr);

}

std::shared_ptr<Mesh> MeshFactory::createQuad()
{
    return std::shared_ptr<Mesh>();
}
