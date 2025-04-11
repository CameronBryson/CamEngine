#include "pch.hpp"
#include "VertexBuffer.hpp"
#include "OpenGLUtil.hpp"

VertexBuffer::VertexBuffer(const std::vector<Vertex>& vertices)
{
	GL_CHECK(glGenBuffers(1, &mVBO));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mVBO));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW));
}

VertexBuffer::VertexBuffer(std::vector<Vertex>&& vertices)
{
	GL_CHECK(glGenBuffers(1, &mVBO));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mVBO));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW));
	// No need to keep the vertices data after uploading to GPU
	// Let it be destroyed when this function returns
}

VertexBuffer::~VertexBuffer()
{
	GL_CHECK(glDeleteBuffers(1, &mVBO));
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
	: mVBO(other.mVBO)
{
	// Set the other's ID to 0 to prevent double deletion
	other.mVBO = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
{
	if (this != &other)
	{
		// Clean up existing resources
		GL_CHECK(glDeleteBuffers(1, &mVBO));
		
		// Transfer ownership
		mVBO = other.mVBO;
		other.mVBO = 0;
	}
	return *this;
}

void VertexBuffer::bind() const
{
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mVBO));
}

void VertexBuffer::unbind() const
{
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::setData(const void* data, unsigned int size)
{
	bind();
	GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
}
