#include "pch.hpp"
#include "OpenGLUniformBuffer.hpp"
#include "platform.hpp"
#include "OpenGLUtil.hpp"

OpenGLUniformBuffer::OpenGLUniformBuffer(unsigned int size, unsigned int binding) : mSize(size), mBinding(binding)
{
    // Generate the UBO
    GL_CHECK(glGenBuffers(1, &mRendererID));
    GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, mRendererID));
    GL_CHECK(glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));

    // Bind it to a specific binding point
    GL_CHECK(glBindBufferBase(GL_UNIFORM_BUFFER, binding, mRendererID));

    GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
    GL_CHECK(glDeleteBuffers(1, &mRendererID));
}

void OpenGLUniformBuffer::setData(const void* data, unsigned int size, unsigned int offset)
{
    GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, mRendererID));
    // Update the subrange [offset, offset+size] in the UBO
    GL_CHECK(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
    GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}
