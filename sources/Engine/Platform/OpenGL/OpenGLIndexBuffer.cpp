#include "OpenGLIndexBuffer.hpp"

OpenGLIndexBuffer::OpenGLIndexBuffer(unsigned int* indices, unsigned int count) : mCount(count)
{
	glGenBuffers(1, &mIBO);
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	glDeleteBuffers(1, &mIBO);
}

void OpenGLIndexBuffer::bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
}

void OpenGLIndexBuffer::unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

unsigned int OpenGLIndexBuffer::getCount() const
{
	return mCount;
}
