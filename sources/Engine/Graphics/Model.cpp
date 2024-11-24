#include "Model.hpp"
#include "Mesh.hpp"
#include "Engine/Managers/GraphicsManager.hpp"
Model::Model(const std::vector <std::string>& meshes)
{
	for( const auto& mesh_name : meshes )
	{
		addMesh(mesh_name);
	}
}
void Model::draw(const ShaderProgram& shader, GraphicsManager& graphicsManager) const
{
	for( const auto& mesh : meshes )
	{
		graphicsManager.getMesh(mesh).draw(shader, graphicsManager);
	}
}
void Model::addMesh(const std::string& meshName)
{
	meshes.push_back(meshName);
}
