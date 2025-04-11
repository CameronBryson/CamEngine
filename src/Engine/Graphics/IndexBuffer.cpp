#include "pch.hpp"
#include "IndexBuffer.hpp"
#include "OpenGLUtil.hpp" 

IndexBuffer::IndexBuffer(const std::vector<unsigned>& indices) : mCount(indices.size())
{
	GL_CHECK(glGenBuffers(1, &mEBO));
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mCount * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW));
}

IndexBuffer::IndexBuffer(std::vector<unsigned>&& indices) : mCount(indices.size())
{
	GL_CHECK(glGenBuffers(1, &mEBO));
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, mCount * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW));
	// No need to keep the indices data after uploading to GPU
	// Let it be destroyed when this function returns
}

IndexBuffer::~IndexBuffer()
{
	GL_CHECK(glDeleteBuffers(1, &mEBO));
}

IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept
	: mEBO(other.mEBO), mCount(other.mCount)
{
	// Set the other's ID to 0 to prevent double deletion
	other.mEBO = 0;
	other.mCount = 0;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept
{
	if (this != &other)
	{
		// Clean up existing resources
		GL_CHECK(glDeleteBuffers(1, &mEBO));
		
		// Transfer ownership
		mEBO = other.mEBO;
		mCount = other.mCount;
		other.mEBO = 0;
		other.mCount = 0;
	}
	return *this;
}

void IndexBuffer::bind() const
{
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO));
}

void IndexBuffer::unbind() const
{
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

size_t IndexBuffer::getCount() const
{
	return mCount;
}
