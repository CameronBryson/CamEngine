#include "pch.hpp"
#include "ShaderStorageBuffer.hpp"
#include "platform.hpp"
#include "OpenGLUtil.hpp"

ShaderStorageBuffer::ShaderStorageBuffer(unsigned int size, unsigned int binding)
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

ShaderStorageBuffer::~ShaderStorageBuffer()
{
    GL_CHECK(glDeleteBuffers(1, &m_ID));
}

void ShaderStorageBuffer::bind(unsigned int binding) const
{
    GL_CHECK(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_ID));
}

void ShaderStorageBuffer::unbind() const
{
    GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}

void ShaderStorageBuffer::setData(const void* data, unsigned int size)
{
    GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ID));
    GL_CHECK(glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, data));
    GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}

unsigned int ShaderStorageBuffer::getID() const
{
    return m_ID;
}

void ShaderStorageBuffer::clear()
{
    // Use glClearBufferData for more efficient clearing
    GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ID));
    GL_CHECK(glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, nullptr));
    GL_CHECK(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}

