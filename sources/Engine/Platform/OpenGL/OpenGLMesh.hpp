#pragma once
#include <vector>
#include <string>
#include <memory>

#include "Engine/Graphics/Mesh.hpp"
class Vertex;
class VertexArray;
class GraphicsManager;
class Shader;
class OpenGLMesh : public Mesh
{
public:
	explicit OpenGLMesh(const std::vector<Vertex>& vertices,const std::vector<unsigned> indices, const std::string& materialName);
	~OpenGLMesh() override;

	void draw(const Shader& shader, GraphicsManager& graphicsManager) const override;

	void setMaterial(const std::string& name) override;
	int getVertexCount() const override;
	int getIndexCount() const override;

private:
	size_t mIndexCount;
	size_t mVertexCount;
	std::shared_ptr<VertexArray> mVertexArray;
	std::string material_name;
};
