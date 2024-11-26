#include "OpenGLVertexBuffer.hpp"

OpenGLVertexBuffer::OpenGLVertexBuffer(const std::vector<Vertex>& vertices)
{
	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	glDeleteBuffers(1, &mVBO);
}

void OpenGLVertexBuffer::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
}

void OpenGLVertexBuffer::unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVertexBuffer::setData(const void* data, unsigned int size)
{
	bind();
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}
