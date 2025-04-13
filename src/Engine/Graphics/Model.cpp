#include "pch.hpp"
#include "Model.hpp"
#include "Mesh.hpp"
#include "glm/ext/matrix_float4x4_precision.hpp"
#include "Engine/Util/Logging.hpp"
#include "Shader.hpp"

Model::Model(std::vector<MeshInstance> meshes) : mMeshes(std::move(meshes))
{
	LOG_TRACE(logging::gGraphicsLogger, "Model '{}' created with {} mesh instances.", mName, mMeshes.size());
	// Optional: Add validation if needed (e.g., check if any mesh pointers are null)
}

void Model::draw(glm::mat4 model) const
{
	for (const auto& meshInstance : mMeshes)
	{
		if (!meshInstance.mesh) {
			LOG_WARN(logging::gGraphicsLogger, "Skipping draw for null mesh in Model '{}'.", mName);
			continue;
		}
		glm::mat4 finalModelMatrix = model * meshInstance.localTransform;
		meshInstance.mesh->draw(finalModelMatrix);
	}
}

void Model::draw(std::shared_ptr<Shader>& shadowShader, glm::mat4 model, bool bindMaterial) const
{
	if (!shadowShader) {
		LOG_ERROR(logging::gGraphicsLogger, "Attempted to shadow draw Model '{}' with a null shader.", mName);
		return;
	}
	for (const auto& meshInstance : mMeshes)
	{
		if (!meshInstance.mesh) {
			LOG_WARN(logging::gGraphicsLogger, "Skipping shadow draw for null mesh in Model '{}'.", mName);
			continue;
		}
		glm::mat4 finalModelMatrix = model * meshInstance.localTransform;
		meshInstance.mesh->draw(shadowShader, finalModelMatrix, bindMaterial);
	}
}

void Model::addMesh(MeshInstance&& mesh)
{
	LOG_TRACE(logging::gGraphicsLogger, "Adding mesh '{}' to Model '{}'.", mesh.mesh ? mesh.mesh->getName() : "<null>", mName);
	mMeshes.push_back(std::move(mesh));
}

const std::vector<MeshInstance>& Model::getMeshes() const
{
	return mMeshes;
}

std::vector<MeshInstance>& Model::getMeshes()
{
	return mMeshes;
}

