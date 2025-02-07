#include "pch.hpp"
#include "OpenGLMesh.hpp"
#include "Engine/Util/platform.hpp"
#include "Shader.hpp"
#include "Material.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"


OpenGLMesh::OpenGLMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned> indices, const std::shared_ptr<Material>& material) : mIndexCount(indices.size()), mVertexCount(vertices.size()), mMaterial(material)
{
	mVertexArray = VertexArray::create();
	mVertexArray->addVertexBuffer(VertexBuffer::create(vertices));
	mVertexArray->setIndexBuffer(IndexBuffer::create(indices));
	//Init VAO
	//Bind VAO
	//Attach VBO to VAO
	//Attach EBO TO VAO
}

OpenGLMesh::~OpenGLMesh()
{

}

void OpenGLMesh::draw(const Shader& shader) const
{
	shader.use();
	mVertexArray->bind();
	if (mMaterial)
		mMaterial->bind(shader);
	glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, nullptr);
	if (mMaterial)
		mMaterial->unbind();
	mVertexArray->unbind();
}


void OpenGLMesh::setMaterial(const std::shared_ptr<Material>& material)
{
	mMaterial = material;
}

int OpenGLMesh::getVertexCount() const
{
	return mVertexCount;
}

int OpenGLMesh::getIndexCount() const
{
	return mIndexCount;
}
