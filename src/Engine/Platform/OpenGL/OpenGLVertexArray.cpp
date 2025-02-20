#include "pch.hpp"
#include "OpenGLVertexArray.hpp"
#include "IndexBuffer.hpp"
#include "Engine/Util/platform.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include "VertexBuffer.hpp"
#include "OpenGLUtil.hpp"

OpenGLVertexArray::OpenGLVertexArray()
{
	GL_CHECK(glGenVertexArrays(1, &mVAO));
}

OpenGLVertexArray::~OpenGLVertexArray()
{
	GL_CHECK(glDeleteVertexArrays(1, &mVAO));
}

void OpenGLVertexArray::bind() const
{
	GL_CHECK(glBindVertexArray(mVAO));
	if (mIndexBuffer)
		mIndexBuffer->bind();
}

void OpenGLVertexArray::unbind() const
{
	GL_CHECK(glBindVertexArray(0));
}

void OpenGLVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
	bind();
	vertexBuffer->bind();

	// Position Attribute
	GL_CHECK(glEnableVertexAttribArray(0));
	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)));

	// Normal Attribute
	GL_CHECK(glEnableVertexAttribArray(1));
	GL_CHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)));

	// Texture Coordinates Attribute
	GL_CHECK(glEnableVertexAttribArray(2));
	GL_CHECK(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coordinates)));

	// Tangent Attribute
	GL_CHECK(glEnableVertexAttribArray(3));
	GL_CHECK(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent)));

	// Bitangent Attribute
	GL_CHECK(glEnableVertexAttribArray(4));
	GL_CHECK(glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent)));

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
