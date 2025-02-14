#pragma once
#include <memory>
#include <vector>
#include <string>
#include <glm/fwd.hpp>
#include "Mesh.hpp"
class Shader;
class GraphicsManager;

class Model
{
public:

	virtual void draw(glm::mat4 model) const = 0;
	virtual void drawShadow(std::shared_ptr<Shader>& shadowShader, glm::mat4 model) const = 0;

	virtual void addMesh(const MeshInstance& mesh) = 0;

	static std::shared_ptr<Model> createModel(const std::vector<MeshInstance>& meshes);
};
