#include "OpenGLVertexArray.hpp"
#include "Engine/Graphics/IndexBuffer.hpp"
#include "Engine/Util/platform.hpp"
#include "Engine/Graphics/Vertex.hpp"
OpenGLVertexArray::OpenGLVertexArray()
{
	glGenVertexArrays(1, &mVAO);
}
OpenGLVertexArray::~OpenGLVertexArray()
{
	glDeleteVertexArrays(1, &mVAO);
}
void OpenGLVertexArray::bind() const
{
	glBindVertexArray(mVAO);
	if (mIndexBuffer)
		mIndexBuffer->bind();
}
void OpenGLVertexArray::unbind() const
{
	glBindVertexArray(0);
}
void OpenGLVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
	bind();
	vertexBuffer->bind();

	glEnableVertexAttribArray(0);
	// Position Attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	// Normal Attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	// Texture Coordinates Attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coordinates));

	// Tangent Attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

	// Bitangent Attribute
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

	mVertexBuffers.push_back(vertexBuffer);

	unbind();
}
const std::vector<std::shared_ptr<VertexBuffer>>& OpenGLVertexArray::getVertexBuffers() const
{
	return mVertexBuffers;
}

void OpenGLVertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
	bind();
	indexBuffer->bind();
	mIndexBuffer = indexBuffer;
}

const std::shared_ptr<IndexBuffer>& OpenGLVertexArray::getIndexBuffer() const
{
	return mIndexBuffer;
}
