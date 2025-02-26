#include "pch.hpp"
#include "OpenGLMesh.hpp"
#include "Engine/Util/platform.hpp"
#include "Shader.hpp"
#include "Material.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "glm/ext/matrix_float4x4_precision.hpp"
#include <OpenGLUtil.hpp>


OpenGLMesh::OpenGLMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned> indices, const std::shared_ptr<Material>& material) : mMaterial(material), mVertices(vertices)
{
	mVertexArray = VertexArray::create();
	mVertexArray->addVertexBuffer(VertexBuffer::create(vertices));
	mVertexArray->setIndexBuffer(IndexBuffer::create(indices));
	calculateBoundingSphere();
}

OpenGLMesh::~OpenGLMesh()
{

}

void OpenGLMesh::draw(glm::mat4 model) const
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

void OpenGLMesh::draw(std::shared_ptr<Shader>& shadowShader, glm::mat4 model, bool bindMaterial) const
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



void OpenGLMesh::setMaterial(const std::shared_ptr<Material>& material)
{
	mMaterial = material;
}

std::shared_ptr<Material> OpenGLMesh::getMaterial()
{
	return mMaterial;
}

std::vector<Vertex>& OpenGLMesh::getVertices()
{
	return mVertices;
}

glm::vec3 OpenGLMesh::getBoundingSphereCenter() const
{
	return mBoundingSphereCenter;
}

float OpenGLMesh::getBoundingSphereRadius() const
{
	return mBoundingSphereRadius;
}

void OpenGLMesh::calculateBoundingSphere()
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



