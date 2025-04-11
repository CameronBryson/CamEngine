#pragma once
#include <vector>
#include <string>
#include <memory>
#include <glm/ext/matrix_float4x4.hpp>
#include <Vertex.hpp>

class VertexArray;
class Shader;
class Material;
class Mesh
{
public:
	Mesh(const std::vector<Vertex>& vertices,const std::vector<unsigned>& indices, const std::shared_ptr<Material>& material);
	~Mesh() ;

	// Delete copy operations to prevent accidental resource duplication
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	// Add move semantics
	Mesh(Mesh&& other) noexcept;
	Mesh& operator=(Mesh&& other) noexcept;

	void draw(glm::mat4 model) const ;
	void draw(std::shared_ptr<Shader>& shadowShader, glm::mat4 model, bool bindMaterial = false) const ;

	void setMaterial(const std::shared_ptr<Material>& material) ;
	std::shared_ptr<Material> getMaterial() ;
	std::vector<Vertex>& getVertices() ;
	glm::vec3 getBoundingSphereCenter() const;
	float getBoundingSphereRadius() const;
	void calculateBoundingSphere();

	void setName(const std::string& name)  { mName = name; }
	const std::string& getName() const  { return mName; }

private:
	std::vector<Vertex> mVertices;
	std::shared_ptr<VertexArray> mVertexArray;
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