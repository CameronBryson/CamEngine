#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Engine/Graphics/Vertex.hpp"
#include <glm/fwd.hpp>
#include <glm/ext/matrix_common.hpp>
class GraphicsManager;
class Material;
class Shader;

class Mesh
{
public:
	virtual ~Mesh() = default;

	virtual void draw(glm::mat4 model) const = 0;
	virtual void draw(std::shared_ptr<Shader>& shadowShader, glm::mat4 model, bool bindMaterial = false) const = 0;

	virtual void setMaterial(const std::shared_ptr<Material>& material) = 0;
	virtual std::shared_ptr<Material> getMaterial() = 0;
	virtual std::vector<Vertex>& getVertices() = 0;	
	virtual glm::vec3 getBoundingSphereCenter() const = 0;
	virtual float getBoundingSphereRadius() const = 0;
	virtual void calculateBoundingSphere() = 0;
	static std::shared_ptr<Mesh> createMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices, const std::shared_ptr<Material>& material);
};
struct MeshInstance
{
	std::shared_ptr<Mesh> mesh;
	glm::mat4 localTransform;
};