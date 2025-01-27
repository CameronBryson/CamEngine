#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Engine/Graphics/Vertex.hpp"
class GraphicsManager;
class Material;
class Shader;
class Mesh
{
public:
	virtual ~Mesh() = default;

	virtual void draw(const Shader& shader) const = 0;

	virtual void setMaterial(const std::shared_ptr<Material>& material) = 0;
	virtual int getVertexCount() const = 0;
	virtual int getIndexCount() const = 0;
	static std::shared_ptr<Mesh> createMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices, const std::shared_ptr<Material>& material);
};
