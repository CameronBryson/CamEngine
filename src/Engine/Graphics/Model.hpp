#pragma once
#include <vector>
#include <string>

#include "Mesh.hpp"
class Shader;
class Model
{
public:
	Model(const std::vector<MeshInstance>& meshes);

	void draw(glm::mat4 model) const;
	void draw(std::shared_ptr<Shader>& shadowShader, glm::mat4 model, bool bindMatieral = false) const;

	void addMesh(const MeshInstance& mesh);
	std::vector<MeshInstance>& getMeshes();

	void setName(const std::string& name) { mName = name; }
	const std::string& getName() const { return mName; }

private:
	std::vector<MeshInstance> mMeshes;
	std::string mName;
};
