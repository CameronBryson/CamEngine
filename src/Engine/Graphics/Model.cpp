#include "pch.hpp"
#include "Model.hpp"
#include "Mesh.hpp"
#include "glm/ext/matrix_float4x4_precision.hpp"

Model::Model(const std::vector<MeshInstance>& meshes)
{
	for (const auto& mesh_name : meshes)
	{
		addMesh(mesh_name);
	}
}

void Model::draw(glm::mat4 model) const
{
	for (const auto& mesh : mMeshes)
	{
		glm::mat4 finalModelMatrix = model * mesh.localTransform;
		mesh.mesh->draw(finalModelMatrix);
	}
}

void Model::draw(std::shared_ptr<Shader>& shadowShader, glm::mat4 model, bool bindMaterial) const
{
	for (const auto& mesh : mMeshes)
	{
		glm::mat4 finalModelMatrix = model * mesh.localTransform;
		mesh.mesh->draw(shadowShader, finalModelMatrix,bindMaterial);
	}
}


void Model::addMesh(const MeshInstance& mesh)
{
	mMeshes.emplace_back(mesh);
}

std::vector<MeshInstance>& Model::getMeshes()
{
	return mMeshes;
}

