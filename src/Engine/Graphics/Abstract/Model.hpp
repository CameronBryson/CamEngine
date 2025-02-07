#pragma once
#include <memory>
#include <vector>
#include <string>
#include <glm/fwd.hpp>

class Shader;
class GraphicsManager;
class Mesh;
class Model
{
public:

	virtual void draw(glm::mat4 model) const = 0;

	virtual void addMesh(const std::shared_ptr<Mesh>& mesh) = 0;

	static std::shared_ptr<Model> createModel(const std::vector<std::shared_ptr<Mesh>>& meshes);
};
