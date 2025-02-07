#include "pch.hpp"
#include "OpenGLUniformBuffer.hpp"

OpenGLUniformBuffer::OpenGLUniformBuffer(unsigned int size, unsigned int binding)
    : mSize(size), mBinding(binding)
{
    glGenBuffers(1, &mRendererID);
    glBindBuffer(GL_UNIFORM_BUFFER, mRendererID);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, mRendererID);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
    glDeleteBuffers(1, &mRendererID);
}

void OpenGLUniformBuffer::setData(const void* data, unsigned int size, unsigned int offset)
{
    glBindBuffer(GL_UNIFORM_BUFFER, mRendererID);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
