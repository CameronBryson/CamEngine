#include "Model.hpp"
#include "Mesh.hpp"
Model::Model(const std::vector <std::string>& meshes)
{
    for( const auto& mesh_name : meshes )
    {
	addMesh(mesh_name);
    }
}
void Model::draw(ShaderProgram& shader, GraphicsManager& graphics_manager)
{
    for( auto& mesh : meshes )
    {
	graphics_manager.getMesh(mesh).draw(shader, graphics_manager);
    }
}
void Model::addMesh(const std::string& mesh_name)
{
    meshes.push_back(mesh_name);
}
