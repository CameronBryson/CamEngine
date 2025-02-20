#include "pch.hpp"
#include "OpenGLVertexBuffer.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include "OpenGLUtil.hpp"

OpenGLVertexBuffer::OpenGLVertexBuffer(const std::vector<Vertex>& vertices)
{
    GL_CHECK(glGenBuffers(1, &mVBO));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mVBO));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW));
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    GL_CHECK(glDeleteBuffers(1, &mVBO));
}

void OpenGLVertexBuffer::bind() const
{
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, mVBO));
}

void OpenGLVertexBuffer::unbind() const
{
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void OpenGLVertexBuffer::setData(const void* data, unsigned int size)
{
    bind();
    GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
}
