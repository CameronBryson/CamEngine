#include "pch.hpp"
#include "Mesh.hpp"
#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"
#include "platform.hpp"
#include "VertexArray.hpp"
#include "Material.hpp"
#include "OpenGLUtil.hpp"
#include "Vertex.hpp"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices, const std::shared_ptr<Material>& material) : mMaterial(material), mVertices(vertices)
{
	mVertexArray = std::make_shared<VertexArray>();
	mVertexArray->addVertexBuffer(std::make_shared<VertexBuffer>(vertices));
	mVertexArray->setIndexBuffer(std::make_shared<IndexBuffer>(indices));
	calculateBoundingSphere();
}

Mesh::~Mesh()
{

}

void Mesh::draw(glm::mat4 model) const
{
	if (!mMaterial)
		return;


	mMaterial->getShader()->use();
	mMaterial->getShader()->setMat4("model", model);
	mMaterial->bind(mMaterial->getShader());

	mVertexArray->bind();


	// Retrieve index count from IndexBuffer
	GL_CHECK(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mVertexArray->getIndexBuffer()->getCount()), GL_UNSIGNED_INT, nullptr));

	mVertexArray->unbind();
	mMaterial->unbind();
}

void Mesh::draw(std::shared_ptr<Shader>& shadowShader, glm::mat4 model, bool bindMaterial) const
{
	if (bindMaterial) 
	{
		if (!mMaterial) 
		{
			return;
		}
	}
	shadowShader->use();
	shadowShader->setMat4("model", model);
	if (bindMaterial)
		mMaterial->bind(shadowShader);

	mVertexArray->bind();

	GL_CHECK(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mVertexArray->getIndexBuffer()->getCount()), GL_UNSIGNED_INT, nullptr));
	mVertexArray->unbind();
	if (bindMaterial)
		mMaterial->unbind();
}



void Mesh::setMaterial(const std::shared_ptr<Material>& material)
{
	mMaterial = material;
}

std::shared_ptr<Material> Mesh::getMaterial()
{
	return mMaterial;
}

std::vector<Vertex>& Mesh::getVertices()
{
	return mVertices;
}

glm::vec3 Mesh::getBoundingSphereCenter() const
{
	return mBoundingSphereCenter;
}

float Mesh::getBoundingSphereRadius() const
{
	return mBoundingSphereRadius;
}

void Mesh::calculateBoundingSphere()
{
	if (mVertices.empty())
	{
		mBoundingSphereCenter = glm::vec3(0.0f);
		mBoundingSphereRadius = 0.0f;
		return;
	}

	// Find AABB first for better initial center
	glm::vec3 minPos = mVertices[0].position;
	glm::vec3 maxPos = mVertices[0].position;

	for (const auto& vertex : mVertices)
	{
		minPos = glm::min(minPos, vertex.position);
		maxPos = glm::max(maxPos, vertex.position);
	}

	// Use AABB center as initial sphere center
	mBoundingSphereCenter = (minPos + maxPos) * 0.5f;

	// Find the vertex furthest from center
	mBoundingSphereRadius = 0.0f;
	for (const auto& vertex : mVertices)
	{
		float distance = glm::length(vertex.position - mBoundingSphereCenter);
		mBoundingSphereRadius = std::max(mBoundingSphereRadius, distance);
	}

	// Add small padding to ensure complete coverage
	mBoundingSphereRadius *= 1.01f; // Adjust this value through ImGui
}



