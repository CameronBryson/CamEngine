#include "Mesh.hpp" 
mesh::mesh(const std::vector <vertex>& vertices, std::string material_name) : vertices(vertices), material_name(std::move(material_name))
{
    index_count = vertices.size();
    setup_mesh();
}
void mesh::setup_mesh()
{
    opengl_util::setup_mesh(vertices, VAO, VBO);
}
void mesh::draw(shader_program& shader, graphics_manager& graphics_manager) const
{
    opengl_util::draw_mesh(shader, graphics_manager, material_name, VAO, index_count);
}
void mesh::set_material(const std::string& name)
{
    material_name = name;
}
