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
     * @brief Adds a vertex buffer and configures its attributes
     * @param vertexBuffer The vertex buffer to add to this VAO
     */
    void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
    
    /**
     * @brief Gets all attached vertex buffers
     * @return Vector of vertex buffer pointers
     */
    const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const;

    /**
     * @brief Sets the index buffer for this vertex array
     * @param indexBuffer The index buffer to use
     */
    void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);
    
    /**
     * @brief Gets the current index buffer
     * @return The current index buffer or nullptr if none is set
     */
    const std::shared_ptr<IndexBuffer>& getIndexBuffer() const;
    
private:
    GLuint mVAO; ///< OpenGL vertex array ID
    std::vector<std::shared_ptr<VertexBuffer>> mVertexBuffers; ///< Associated vertex buffers
    std::shared_ptr<IndexBuffer> mIndexBuffer; ///< Associated index buffer
};