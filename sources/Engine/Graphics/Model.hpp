#pragma once
#include <memory>
#include <vector>
#include <string>
class Shader;
class GraphicsManager;
class Model
{
public:

	virtual void draw(const Shader& shader, GraphicsManager& graphicsManager) const = 0;

	virtual void addMesh(const std::string& meshName) = 0;

	virtual int getTotalVertexCount() const = 0;

	virtual int getTotalTriangleCount() const = 0;


	static std::shared_ptr<Model> createModel(const std::vector<std::string>& mesh_names);
};
