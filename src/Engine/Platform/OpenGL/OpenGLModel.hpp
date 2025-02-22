#pragma once
#include <vector>
#include <string>

#include "Model.hpp"
#include "Mesh.hpp"
class Shader;
class GraphicsManager;
class OpenGLModel : public Model
{
public:
	explicit OpenGLModel(const std::vector<MeshInstance>& meshes);

	void draw(glm::mat4 model) const override;
	void draw(std::shared_ptr<Shader>& shadowShader, glm::mat4 model, bool bindMatieral = false) const override;

	void addMesh(const MeshInstance& mesh) override;
	std::vector<MeshInstance> getMeshes() override;

private:
	std::vector<MeshInstance> mMeshes;
};
