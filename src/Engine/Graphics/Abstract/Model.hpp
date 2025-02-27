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
	virtual void draw(std::shared_ptr<Shader>& shadowShader, glm::mat4 model, bool bindMatieral = false) const = 0;

	virtual void addMesh(const MeshInstance& mesh) = 0;
	virtual std::vector<MeshInstance>& getMeshes() = 0;
	virtual void setName(const std::string& name) = 0;
	virtual const std::string& getName() const = 0;

	static std::shared_ptr<Model> createModel(const std::vector<MeshInstance>& meshes);
};
