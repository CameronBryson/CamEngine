#include "Mesh.hpp" 
#include "GraphicsManager.hpp"
#include "OpenGLUtil.hpp"

Mesh::Mesh(const std::vector <Vertex>& vertices, const std::string& materialName) : vertices(vertices), material_name(std::move(materialName))
{
    index_count = vertices.size();
    setupMesh();
}
void Mesh::setupMesh()
{
    OpenGlUtil::setupMesh(vertices, VAO, VBO);
}
void Mesh::draw(const ShaderProgram& shader, GraphicsManager& graphicsManager) const
{
    OpenGlUtil::drawMesh(shader, graphicsManager, material_name, VAO, index_count);
}
void Mesh::setMaterial(const std::string& name)
{
    material_name = name;
}
