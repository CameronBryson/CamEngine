#include "pch.hpp"
#include "OpenGLModel.hpp"
#include "Mesh.hpp"
#include "glm/ext/matrix_float4x4_precision.hpp"

OpenGLModel::OpenGLModel(const std::vector<MeshInstance>& meshes)
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
		glm::mat4 finalModelMatrix = model * mesh.localTransform;
		mesh.mesh->draw(finalModelMatrix);
	}
}

void OpenGLModel::drawShadow(std::shared_ptr<Shader>& shadowShader, glm::mat4 model) const
{
	for (const auto& mesh : mMeshes)
	{
		glm::mat4 finalModelMatrix = model * mesh.localTransform;
		mesh.mesh->drawShadow(shadowShader, finalModelMatrix);
	}
}


void OpenGLModel::addMesh(const MeshInstance& mesh)
{
	mMeshes.emplace_back(mesh);
}

std::vector<MeshInstance> OpenGLModel::getMeshes()
{
	return mMeshes;
}

