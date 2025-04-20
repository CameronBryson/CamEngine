#pragma once
#include <memory>
#include <vector>
#include "Engine/Util/platform.hpp"

/**
 * @class IndexBuffer
 * @brief Wrapper for OpenGL Element Buffer Object (EBO)
 *
 * Manages index buffer data for indexed geometry rendering.
 * Stores indices that reference vertices in a vertex buffer.
 */
class IndexBuffer
{
public:
	/**
	 * @brief Constructs an index buffer using index data.
	 *        Takes the vector by value for optimal handling of both
	 *        lvalues (copy) and rvalues (move).
	 * @param indices Vector of index data
	 */
	explicit IndexBuffer(const std::vector<unsigned>& indices);

	/**
	 * @brief Destructor - cleans up OpenGL resources
	 */
	~IndexBuffer();

	// Delete copy constructor and assignment operator
	IndexBuffer(const IndexBuffer&) = delete;
	IndexBuffer& operator=(const IndexBuffer&) = delete;

	// Allow move operations
	IndexBuffer(IndexBuffer&& other) noexcept;
	IndexBuffer& operator=(IndexBuffer&& other) noexcept;

	/**
	 * @brief Binds this index buffer for rendering or modification
	 */
	void bind() const;

	/**
	 * @brief Unbinds this index buffer
	 */
	static void unbind();

	/**
	 * @brief Gets the number of indices in this buffer
	 * @return Count of indices
	 */
	size_t getCount() const;

private:
	GLuint mEBO;          ///< OpenGL element buffer ID
	size_t mCount;  ///< Number of indices in the buffer
};