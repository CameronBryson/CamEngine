#include "pch.hpp"
#include "OpenGLUniformBuffer.hpp"
#include "platform.hpp"
OpenGLUniformBuffer::OpenGLUniformBuffer(unsigned int size, unsigned int binding) : mSize(size), mBinding(binding)
{
    // Generate the UBO
    glGenBuffers(1, &mRendererID);
    glBindBuffer(GL_UNIFORM_BUFFER, mRendererID);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

    // Bind it to a specific binding point
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
    // Update the subrange [offset, offset+size] in the UBO
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
