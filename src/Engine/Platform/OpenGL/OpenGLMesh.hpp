#pragma once
#include <vector>
#include <string>
#include <memory>

#include "Mesh.hpp"
class Vertex;
class VertexArray;
class GraphicsManager;
class Shader;
class Material;
class OpenGLMesh : public Mesh
{
public:
	explicit OpenGLMesh(const std::vector<Vertex>& vertices,const std::vector<unsigned> indices, const std::shared_ptr<Material>& material);
	~OpenGLMesh() override;

	void draw(glm::mat4 model) const override;
	void drawShadow(std::shared_ptr<Shader>& shadowShader, glm::mat4 model) const override;

	void setMaterial(const std::shared_ptr<Material>& material) override;
	std::vector<Vertex>& getVertices() override;

private:
	std::vector<Vertex> mVertices;
	std::shared_ptr<VertexArray> mVertexArray;
	std::shared_ptr<Material> mMaterial;
};
