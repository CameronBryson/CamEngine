#pragma once
#include <vector>
#include <string>

#include "Engine/Graphics/Mesh.hpp"
#include "Engine/Graphics/Vertex.hpp"
class GraphicsManager;
class Shader;
class OpenGLMesh : public Mesh
{
public:
	explicit OpenGLMesh(const std::vector<Vertex>& vertices, const std::string& materialName);

	void setupMesh() override;

	void draw(const Shader& shader, GraphicsManager& graphicsManager) const override;

	void setMaterial(const std::string& name) override;
private:
	unsigned int VAO = 0, VBO = 0, index_count;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::string material_name;
};
