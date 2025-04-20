#pragma once
#include <vector>
#include <string>
#include <string_view>
#include <memory>

#include "Mesh.hpp"
class Shader;
class Model
{
public:
	Model(std::vector<MeshInstance> meshes);
	Model() = default;

	void draw(const glm::mat4& model) const;
	void draw(const std::shared_ptr<Shader>& shadowShader, const glm::mat4& model, bool bindMaterial = false) const;

	void addMesh(MeshInstance&& mesh);
	const std::vector<MeshInstance>& getMeshes() const;
	std::vector<MeshInstance>& getMeshes();

	void setName(std::string_view name) { mName = name; }
	const std::string& getName() const { return mName; }

private:
	std::vector<MeshInstance> mMeshes;
	std::string mName;
};
