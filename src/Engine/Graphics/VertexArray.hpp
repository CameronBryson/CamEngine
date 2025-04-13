#pragma once
#include <memory>
#include <vector>
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "Engine/Util/platform.hpp"

/**
 * @class VertexArray
 * @brief Wrapper for OpenGL Vertex Array Object (VAO)
 *
 * Manages the creation, binding, and configuration of vertex array objects,
 * which store the vertex attribute configuration and associated vertex/index buffers.
 */
class VertexArray
{
public:
    /**
     * @brief Constructor - creates a new vertex array object
     */
    VertexArray();
    
    /**
     * @brief Destructor - cleans up OpenGL resources
     */
    ~VertexArray();
    
    /**
     * @brief Move constructor
     * @param other The VertexArray to move from
     */
    VertexArray(VertexArray&& other) noexcept;
    
    /**
     * @brief Move assignment operator
     * @param other The VertexArray to move from
     * @return Reference to this VertexArray
     */
    VertexArray& operator=(VertexArray&& other) noexcept;
    
    // Delete copy constructor and copy assignment to prevent resource copying
    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;
    
    /**
     * @brief Binds this vertex array for rendering or modification
     */
    void bind() const;
    
    /**
     * @brief Unbinds this vertex array
     */
    void unbind() const;
    
    /**
     * @brief Adds a vertex buffer, taking ownership, and configures its attributes.
     * @param vertexBuffer Rvalue reference to a unique pointer to the vertex buffer.
     */
    void addVertexBuffer(std::unique_ptr<VertexBuffer>&& vertexBuffer);
    
    /**
     * @brief Gets references to the attached vertex buffers.
     * @return Const reference to the vector of vertex buffer unique pointers.
     */
    const std::vector<std::unique_ptr<VertexBuffer>>& getVertexBuffers() const;

    /**
     * @brief Sets the index buffer for this vertex array, taking ownership.
     * @param indexBuffer Rvalue reference to a unique pointer to the index buffer.
     */
    void setIndexBuffer(std::unique_ptr<IndexBuffer>&& indexBuffer);
    
    /**
     * @brief Gets a reference to the current index buffer.
     * @return Const reference to the unique pointer of the index buffer (or null if none).
     */
    const std::unique_ptr<IndexBuffer>& getIndexBuffer() const;
    
private:
    GLuint mVAO; ///< OpenGL vertex array ID
    std::vector<std::unique_ptr<VertexBuffer>> mVertexBuffers; ///< Owned vertex buffers
    std::unique_ptr<IndexBuffer> mIndexBuffer; ///< Owned index buffer
};