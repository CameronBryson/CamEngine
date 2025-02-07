#pragma once
#include <memory>
#include <vector>
#include <string>
class Shader;
class GraphicsManager;
class Mesh;
class Model
{
public:

	virtual void draw(const Shader& shader) const = 0;

	virtual void addMesh(const std::shared_ptr<Mesh>& mesh) = 0;

	virtual int getTotalVertexCount() const = 0;

	virtual int getTotalTriangleCount() const = 0;


	static std::shared_ptr<Model> createModel(const std::vector<std::shared_ptr<Mesh>>& meshes);
};
