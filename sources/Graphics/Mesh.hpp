#pragma once
#include <utility>

#include "Material.hpp"
#include "OpenGLUtil.hpp"
#include "Vertex.hpp"
#include "GraphicsManager.hpp"
class mesh
{
public:
    mesh(const std::vector<vertex>& vertices, std::string  material_name) : vertices(vertices), material_name(std::move(material_name))
    {
        index_count = vertices.size();
        setup_mesh();
    }

    void setup_mesh()
    {
        opengl_util::setup_mesh(vertices, VAO, VBO);
    }

    void draw(shader_program& shader, graphics_manager& graphics_manager) const
    {
        opengl_util::draw_mesh(shader, graphics_manager, material_name, VAO, index_count);
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
