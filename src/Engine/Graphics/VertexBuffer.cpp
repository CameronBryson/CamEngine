#include "pch.hpp"
#include "VertexBuffer.hpp"
#include "OpenGLUtil.hpp"

VertexBuffer::VertexBuffer(const std::vector<Vertex>& vertices)
{
    GL_CHECK(glGenBuffers(1, &mVBO));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mVBO));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
    GL_CHECK(glDeleteBuffers(1, &mVBO));
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
