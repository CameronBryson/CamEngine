#include "OpenGLModel.hpp"
#include "Engine/Managers/GraphicsManager.hpp"
#include "Engine/Graphics/Mesh.hpp"

OpenGLModel::OpenGLModel(const std::vector<std::string>& meshes)
{
	for (const auto& mesh_name : meshes)
	{
		addMesh(mesh_name);
	}
}

void OpenGLModel::draw(const Shader& shader, GraphicsManager& graphicsManager) const
{
	for (const auto& mesh : meshes)
	{
		graphicsManager.getMesh(mesh)->draw(shader, graphicsManager);
	}
}

void OpenGLModel::addMesh(const std::string& meshName)
{
	meshes.push_back(meshName);

}

int OpenGLModel::getTotalVertexCount() const
{
	return mTotalVertexCount;
}

int OpenGLModel::getTotalTriangleCount() const
{
	return mTotalVertexCount / 3;
}
