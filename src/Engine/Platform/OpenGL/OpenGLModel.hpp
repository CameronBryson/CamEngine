#pragma once
#include <vector>
#include <string>

#include "Engine/Graphics/Model.hpp"
class Mesh;
class Shader;
class GraphicsManager;
class OpenGLModel : public Model
{
public:
	explicit OpenGLModel(const std::vector<std::shared_ptr<Mesh>>& meshes);

	void draw(const Shader& shader) const override;

	void addMesh(const std::shared_ptr<Mesh>& mesh) override;
	int getTotalVertexCount() const override;
	int getTotalTriangleCount() const override;

private:
	//std::vector<std::string> meshes;
	int mTotalVertexCount = 0;
	std::vector<std::shared_ptr<Mesh>> mMeshes;
};
