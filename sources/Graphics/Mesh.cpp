#include "Mesh.hpp" 
Mesh::Mesh(const std::vector <Vertex>& vertices, const std::string& material_name) : vertices(vertices), material_name(std::move(material_name))
{
    index_count = vertices.size();
    setupMesh();
}
void Mesh::setupMesh()
{
    OpenGlUtil::setupMesh(vertices, VAO, VBO);
}
void Mesh::draw(ShaderProgram& shader, GraphicsManager& graphics_manager) const
{
    OpenGlUtil::drawMesh(shader, graphics_manager, material_name, VAO, index_count);
}
void Mesh::setMaterial(const std::string& name)
{
    material_name = name;
}
