#pragma once
#include <vector>
#include <string>
#include <memory>
#include <string_view>
#include <glm/ext/matrix_float4x4.hpp>
#include <Vertex.hpp>

class VertexArray;
class Shader;
class Material;
class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, const std::vector<unsigned>& indices, std::shared_ptr<Material> material);
	~Mesh();

	// Delete copy operations to prevent accidental resource duplication
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	// Add move semantics
	Mesh(Mesh&& other) noexcept;
	Mesh& operator=(Mesh&& other) noexcept;

	void draw(const glm::mat4& model) const;
	void draw(const std::shared_ptr<Shader>& shadowShader, const glm::mat4& model, bool bindMaterial = false) const;

	void setMaterial(const std::shared_ptr<Material>& material);
	std::shared_ptr<Material> getMaterial() const;
	const std::vector<Vertex>& getVertices() const;
	glm::vec3 getBoundingSphereCenter() const;
	float getBoundingSphereRadius() const;
	void calculateBoundingSphere();

	void setName(std::string_view name) { mName = name; }
	const std::string& getName() const { return mName; }

private:
	std::vector<Vertex> mVertices;
	std::unique_ptr<VertexArray> mVertexArray;
	std::shared_ptr<Material> mMaterial;
	glm::vec3 mBoundingSphereCenter;
	float mBoundingSphereRadius;
	std::string mName;
};
struct MeshInstance
{
	std::shared_ptr<Mesh> mesh;
	glm::mat4 localTransform;
};