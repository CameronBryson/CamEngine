#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Engine/Graphics/Vertex.hpp"
#include <glm/fwd.hpp>
class GraphicsManager;
class Material;
class Shader;
class Mesh
{
public:
	virtual ~Mesh() = default;

	virtual void draw(glm::mat4 model) const = 0;

	virtual void setMaterial(const std::shared_ptr<Material>& material) = 0;
	static std::shared_ptr<Mesh> createMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices, const std::shared_ptr<Material>& material);
};
