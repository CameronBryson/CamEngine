#pragma once
#include <memory>
#include <vector>
#include <string>

#include "Vertex.hpp"
class GraphicsManager;
class Shader;
class Mesh
{
public:
	virtual ~Mesh() = default;

	virtual void draw(const Shader& shader, GraphicsManager& graphicsManager) const = 0;

	virtual void setMaterial(const std::string& name) = 0;
	static std::shared_ptr<Mesh> createMesh(const std::vector<Vertex>& vertices, const std::string& material_name);
};
