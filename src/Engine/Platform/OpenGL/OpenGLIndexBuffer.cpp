#include "pch.hpp"
#include "OpenGLIndexBuffer.hpp"
#include "OpenGLUtil.hpp" // Include the header where GL_CHECK is defined

OpenGLIndexBuffer::OpenGLIndexBuffer(const std::vector<unsigned>& indices) : mCount(indices.size())
{
	GL_CHECK(glGenBuffers(1, &mEBO));
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mCount * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW));
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	GL_CHECK(glDeleteBuffers(1, &mEBO));
}

void OpenGLIndexBuffer::bind() const
{
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO));
}

void OpenGLIndexBuffer::unbind() const
{
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

unsigned int OpenGLIndexBuffer::getCount() const
{
	return mCount;
}
