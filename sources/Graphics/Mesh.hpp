#pragma once
#include "GraphicsManager.hpp"
#include "Material.hpp"
#include "Vertex.hpp"

class mesh
{
public:
    mesh(const std::vector<vertex>& vertices) : vertices(vertices)
    {
        index_count = vertices.size();
        setup_mesh();
    }

    void setup_mesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);


        // position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);

        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texture_coordinates));


        glBindVertexArray(0);
    }

    void draw(shader_program& shader)
    {
        graphics_manager::get_material(material_name).bind(shader);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, index_count);
        //glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT,0);
        glBindVertexArray(0);
    }

    void set_material(const std::string& name)
    {
        material_name = name;
    }

private:
    unsigned int VAO = 0, VBO = 0, index_count;
    std::vector<vertex> vertices;
    std::vector<unsigned int> indices;
    std::string material_name;
};
