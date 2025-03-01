#include "pch.hpp"
#include "OpenGLShaderStorageBuffer.hpp"
#include "platform.hpp"
#include "OpenGLUtil.hpp"

OpenGLShaderStorageBuffer::OpenGLShaderStorageBuffer(unsigned int size, unsigned int binding)
    : m_Size(size)
{
    // Generate the SSBO
    GL_CHECK(glGenBuffers(1, &m_ID));
    GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ID));
    GL_CHECK(glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));

    // Bind it to a specific binding point
    GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_ID));

    GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}

OpenGLShaderStorageBuffer::~OpenGLShaderStorageBuffer()
{
    GL_CHECK(glDeleteBuffers(1, &m_ID));
}

void OpenGLShaderStorageBuffer::bind(unsigned int binding) const
{
    GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_ID));
}

void OpenGLShaderStorageBuffer::unbind() const
{
    GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}

void OpenGLShaderStorageBuffer::setData(const void* data, unsigned int size)
{
    GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ID));
    GL_CHECK(glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, data));
    GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}

unsigned int OpenGLShaderStorageBuffer::getID() const
{
    return m_ID;
}

void OpenGLShaderStorageBuffer::clear()
{
    // Use glClearBufferData for more efficient clearing
    GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ID));
    GL_CHECK(glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, nullptr));
    GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}

