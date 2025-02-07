#pragma once
#include <vector>
#include <string>

#include "Model.hpp"
class Mesh;
class Shader;
class GraphicsManager;
class OpenGLModel : public Model
{
public:
	explicit OpenGLModel(const std::vector<std::shared_ptr<Mesh>>& meshes);

	void draw(glm::mat4 model) const override;

	void addMesh(const std::shared_ptr<Mesh>& mesh) override;

private:
	std::vector<std::shared_ptr<Mesh>> mMeshes;
};
