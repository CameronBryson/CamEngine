#include "pch.hpp"
#include "Mesh.hpp"
#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"
#include "platform.hpp"
#include "VertexArray.hpp"
#include "Material.hpp"
#include "OpenGLUtil.hpp"
#include "Vertex.hpp"
#include "Engine/Util/Logging.hpp"
#include "Shader.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, const std::vector<unsigned>& indices, std::shared_ptr<Material> material)
	: mVertices(std::move(vertices)),
	mMaterial(std::move(material))
{
	ASSERT_LOG(logging::gGraphicsLogger, !mVertices.empty() && !indices.empty(),
			   "Attempted to create Mesh '{}' with empty vertices or indices.", mName);

	mVertexArray = std::make_unique<VertexArray>(VertexBuffer(mVertices), IndexBuffer(indices));

	calculateBoundingSphere();
	LOG_TRACE(logging::gGraphicsLogger, "Mesh '{}' created with {} vertices and {} indices.", mName, mVertices.size(), mVertexArray->getIndexBuffer() ? mVertexArray->getIndexBuffer()->getCount() : 0);
}

Mesh::~Mesh()
{
	LOG_TRACE(logging::gGraphicsLogger, "Destroying Mesh '{}'.", mName);
}

// Move constructor implementation
Mesh::Mesh(Mesh&& other) noexcept
	: mVertices(std::move(other.mVertices)),
	mVertexArray(std::move(other.mVertexArray)),
	mMaterial(std::move(other.mMaterial)),
	mBoundingSphereCenter(other.mBoundingSphereCenter),
	mBoundingSphereRadius(other.mBoundingSphereRadius),
	mName(std::move(other.mName))
{
	LOG_TRACE(logging::gGraphicsLogger, "Mesh '{}' move constructed.", mName);
}

// Move assignment operator implementation
Mesh& Mesh::operator=(Mesh&& other) noexcept
{
	if (this != &other)
	{
		LOG_TRACE(logging::gGraphicsLogger, "Mesh '{}' move assigned from '{}'.", mName, other.mName);
		mVertices = std::move(other.mVertices);
		mVertexArray = std::move(other.mVertexArray);
		mMaterial = std::move(other.mMaterial);
		mBoundingSphereCenter = other.mBoundingSphereCenter;
		mBoundingSphereRadius = other.mBoundingSphereRadius;
		mName = std::move(other.mName);

		other.mBoundingSphereCenter = {};
		other.mBoundingSphereRadius = 0.0f;
	}
	return *this;
}

void Mesh::draw(const glm::mat4& model) const
{
	if (!mMaterial)
	{
		LOG_WARN(logging::gGraphicsLogger, "Attempted to draw Mesh '{}' with no material.", mName);
		return;
	}
	if (!mVertexArray)
	{
		LOG_ERROR(logging::gGraphicsLogger, "Attempted to draw Mesh '{}' with no vertex array.", mName);
		return;
	}
	auto shader = mMaterial->getShader();
	if (!shader)
	{
		LOG_WARN(logging::gGraphicsLogger, "Attempted to draw Mesh '{}', but its material '{}' has no shader.", mName, mMaterial->getName());
		return;
	}

	shader->use();
	shader->setMat4("model", model);
	mMaterial->bind(*shader);

	mVertexArray->bind();

	const auto& ib = mVertexArray->getIndexBuffer();
	//if (!ib) {
	//    LOG_ERROR(logging::gGraphicsLogger, "Attempted to draw Mesh '{}', but it has no index buffer.", mName);
	//    mVertexArray->unbind();
	//    mMaterial->unbind();
	//    return;
	//}

	GL_CHECK(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(ib.getCount()), GL_UNSIGNED_INT, nullptr));

	mVertexArray->unbind();
	mMaterial->unbind();
}

void Mesh::draw(const std::shared_ptr<Shader>& shadowShader, const glm::mat4& model, const bool bindMaterial) const
{
	if (!mVertexArray)
	{
		LOG_ERROR(logging::gGraphicsLogger, "Attempted to shadow draw Mesh '{}' with no vertex array.", mName);
		return;
	}
	if (!shadowShader)
	{
		LOG_ERROR(logging::gGraphicsLogger, "Attempted to shadow draw Mesh '{}' with a null shader.", mName);
		return;
	}

	if (bindMaterial)
	{
		if (!mMaterial)
		{
			LOG_WARN(logging::gGraphicsLogger, "Attempted to shadow draw Mesh '{}' with material binding, but no material is set.", mName);
		}
	}

	shadowShader->use();
	shadowShader->setMat4("model", model);

	if (bindMaterial && mMaterial)
	{
		mMaterial->bind(*shadowShader);
	}

	mVertexArray->bind();

	const auto& ib = mVertexArray->getIndexBuffer();
	//if (!ib) {
	//    LOG_ERROR(logging::gGraphicsLogger, "Attempted to shadow draw Mesh '{}', but it has no index buffer.", mName);
	//    mVertexArray->unbind();
	//    if (bindMaterial && mMaterial) mMaterial->unbind();
	//    return;
	//}

	GL_CHECK(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(ib.getCount()), GL_UNSIGNED_INT, nullptr));
	mVertexArray->unbind();
	if (bindMaterial && mMaterial)
	{
		mMaterial->unbind();
	}
}

void Mesh::setMaterial(const std::shared_ptr<Material>& material)
{
	LOG_TRACE(logging::gGraphicsLogger, "Setting material for Mesh '{}'.", mName);
	mMaterial = material;
}

std::shared_ptr<Material> Mesh::getMaterial() const
{
	return mMaterial;
}

const std::vector<Vertex>& Mesh::getVertices() const
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
	LOG_TRACE(logging::gGraphicsLogger, "Calculating bounding sphere for Mesh '{}'.", mName);
	if (mVertices.empty())
	{
		LOG_WARN(logging::gGraphicsLogger, "Cannot calculate bounding sphere for Mesh '{}': No vertices.", mName);
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
	mBoundingSphereRadius *= 1.01f;
	LOG_DEBUG(logging::gGraphicsLogger, "Mesh '{}' bounding sphere calculated: Center=({:.2f}, {:.2f}, {:.2f}), Radius={:.2f}",
			  mName, mBoundingSphereCenter.x, mBoundingSphereCenter.y, mBoundingSphereCenter.z, mBoundingSphereRadius);
}



