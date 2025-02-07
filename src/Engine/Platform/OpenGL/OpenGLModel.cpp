#include "pch.hpp"
#include "OpenGLModel.hpp"
#include "Mesh.hpp"
#include "glm/ext/matrix_float4x4_precision.hpp"

OpenGLModel::OpenGLModel(const std::vector<std::shared_ptr<Mesh>>& meshes)
{
	for (const auto& mesh_name : meshes)
	{
		addMesh(mesh_name);
	}
}

void OpenGLModel::draw(glm::mat4 model) const
{
	for (const auto& mesh : mMeshes)
	{
		mesh->draw(model);
	}
}


void OpenGLModel::addMesh(const std::shared_ptr<Mesh>& meshName)
{
	mMeshes.emplace_back(meshName);
}

