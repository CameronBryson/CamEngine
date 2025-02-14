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
	void drawShadow(std::shared_ptr<Shader>& shadowShader, glm::mat4 model) const override;

	void addMesh(const MeshInstance& mesh) override;
	std::vector<MeshInstance> getMeshes() override;

private:
	std::vector<MeshInstance> mMeshes;
};
