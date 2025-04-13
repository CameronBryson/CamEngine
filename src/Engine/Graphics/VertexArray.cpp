#include "pch.hpp"
#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
#include "Engine/Util/platform.hpp"
#include "Engine/Graphics/Vertex.hpp"
#include "VertexBuffer.hpp"
#include "OpenGLUtil.hpp"

VertexArray::VertexArray()
{
	GL_CHECK(glGenVertexArrays(1, &mVAO));
}

VertexArray::~VertexArray()
{
	GL_CHECK(glDeleteVertexArrays(1, &mVAO));
}

VertexArray::VertexArray(VertexArray&& other) noexcept
    : mVAO(other.mVAO), 
      mVertexBuffers(std::move(other.mVertexBuffers)),
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
        mVertexBuffers = std::move(other.mVertexBuffers);
        mIndexBuffer = std::move(other.mIndexBuffer);

        // Invalidate the other object's VAO handle
        other.mVAO = 0;
    }
    return *this;
}

void VertexArray::bind() const
{
	GL_CHECK(glBindVertexArray(mVAO));
	if (mIndexBuffer)
		mIndexBuffer->bind();
}

void VertexArray::unbind() const
{
	GL_CHECK(glBindVertexArray(0));
    // It's generally good practice to unbind the EBO *after* the VAO
    if (mIndexBuffer) // Though technically unbinding VAO unbinds EBO
        mIndexBuffer->unbind();
}

void VertexArray::addVertexBuffer(std::unique_ptr<VertexBuffer>&& vertexBuffer)
{
    if (!vertexBuffer) return; // Or handle error

	bind();
	vertexBuffer->bind();

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

	mVertexBuffers.push_back(std::move(vertexBuffer)); // Move ownership into the vector

	// Unbind the VBO after setting attributes, but keep VAO bound if needed for further setup
    // vertexBuffer->unbind(); // This line is technically redundant after the move, and unbinding VAO does this
	// unbind(); // Don't unbind here, allow for chaining or setting index buffer
}

const std::vector<std::unique_ptr<VertexBuffer>>& VertexArray::getVertexBuffers() const
{
	return mVertexBuffers;
}

void VertexArray::setIndexBuffer(std::unique_ptr<IndexBuffer>&& indexBuffer)
{
    if (!indexBuffer) return; // Or handle error

	bind();
	indexBuffer->bind(); // EBO binding is stored within the VAO state
	mIndexBuffer = std::move(indexBuffer); // Move ownership

    // No need to explicitly unbind the index buffer here, 
    // as its binding state is part of the VAO state.
    // Unbinding the VAO later will handle it.
}

const std::unique_ptr<IndexBuffer>& VertexArray::getIndexBuffer() const
{
	return mIndexBuffer;
}
