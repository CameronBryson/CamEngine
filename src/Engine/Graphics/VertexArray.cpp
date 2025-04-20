#include "pch.hpp"
#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "Engine/Util/platform.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include "VertexBuffer.hpp"
#include "OpenGLUtil.hpp"

VertexArray::VertexArray(VertexBuffer vertexBuffer, IndexBuffer indexBuffer) : mVertexBuffer(std::move(vertexBuffer)), mIndexBuffer(std::move(indexBuffer))
{
	GL_CHECK(glGenVertexArrays(1, &mVAO));
	GL_CHECK(glBindVertexArray(mVAO));
	mVertexBuffer.bind();

	// Position Attribute
	GL_CHECK(glEnableVertexAttribArray(0));
	GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)));

	// Normal Attribute
	GL_CHECK(glEnableVertexAttribArray(1));
	GL_CHECK(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)));

	// Texture Coordinates Attribute
	GL_CHECK(glEnableVertexAttribArray(2));
	GL_CHECK(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coordinates)));

	// Tangent Attribute
	GL_CHECK(glEnableVertexAttribArray(3));
	GL_CHECK(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent)));

	// Bitangent Attribute
	GL_CHECK(glEnableVertexAttribArray(4));
	GL_CHECK(glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent)));

}

VertexArray::~VertexArray()
{
	GL_CHECK(glDeleteVertexArrays(1, &mVAO));
}

VertexArray::VertexArray(VertexArray&& other) noexcept
	: mVAO(other.mVAO),
	mVertexBuffer(std::move(other.mVertexBuffer)),
	mIndexBuffer(std::move(other.mIndexBuffer))
{
	// Invalidate the other object's VAO handle
	other.mVAO = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
{
	if (this != &other)
	{
		// Delete our current VAO
		if (mVAO != 0)
		{
			GL_CHECK(glDeleteVertexArrays(1, &mVAO));
		}

		// Move resources from other to this
		mVAO = other.mVAO;
		mVertexBuffer = std::move(other.mVertexBuffer);
		mIndexBuffer = std::move(other.mIndexBuffer);

		// Invalidate the other object's VAO handle
		other.mVAO = 0;
	}
	return *this;
}

void VertexArray::bind() const
{
	GL_CHECK(glBindVertexArray(mVAO));
	mIndexBuffer.bind();
}

void VertexArray::unbind() const
{
	GL_CHECK(glBindVertexArray(0));
	mIndexBuffer.unbind();

}

const VertexBuffer& VertexArray::getVertexBuffer() const
{
	return mVertexBuffer;
}


const IndexBuffer& VertexArray::getIndexBuffer() const
{
	return mIndexBuffer;
}
