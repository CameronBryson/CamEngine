#include "pch.hpp"
#include "OpenGLModel.hpp"
#include "Engine/Managers/GraphicsManager.hpp"
#include "Engine/Graphics/Mesh.hpp"

OpenGLModel::OpenGLModel(const std::vector<std::shared_ptr<Mesh>>& meshes)
{
	for (const auto& mesh_name : meshes)
	{
		addMesh(mesh_name);
	}
}

void OpenGLModel::draw(const Shader& shader) const
{
	for (const auto& mesh : mMeshes)
	{
		mesh->draw(shader);
	}
}


void OpenGLModel::addMesh(const std::shared_ptr<Mesh>& meshName)
{
	mTotalVertexCount += meshName->getVertexCount();
	mMeshes.emplace_back(meshName);
}

int OpenGLModel::getTotalVertexCount() const
{
	return mTotalVertexCount;
}

int OpenGLModel::getTotalTriangleCount() const
{
	return mTotalVertexCount / 3;
}
