#include "pch.hpp"
#include "IndexBuffer.hpp"
#include "OpenGLUtil.hpp" 

IndexBuffer::IndexBuffer(const std::vector<unsigned>& indices) : mCount(indices.size())
{
	GL_CHECK(glGenBuffers(1, &mEBO));
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mCount * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
	GL_CHECK(glDeleteBuffers(1, &mEBO));
}

void IndexBuffer::bind() const
{
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO));
}

void IndexBuffer::unbind() const
{
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

unsigned int IndexBuffer::getCount() const
{
	return mCount;
}
