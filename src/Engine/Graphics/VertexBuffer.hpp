#pragma once
#include "Engine/Util/platform.hpp"
#include <vector>
#include <Vertex.hpp>

/**
 * @class VertexBuffer
 * @brief Wrapper for OpenGL Vertex Buffer Object (VBO)
 *
 * Manages the creation, binding, and data transfers for vertex buffers in OpenGL.
 * Handles vertex data storage and transfer to the GPU.
 */
class VertexBuffer
{
public:
    /**
     * @brief Constructs a vertex buffer using vertex data.
     *        Takes the vector by value for optimal handling of both
     *        lvalues (copy) and rvalues (move).
     * @param vertices Vector of vertices to store in the buffer
     */
    explicit VertexBuffer(std::vector<Vertex> vertices);
    
    /**
     * @brief Destructor - cleans up OpenGL resources
     */
    ~VertexBuffer();
    
    // Delete copy constructor and assignment operator
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;
    
    // Allow move operations
    VertexBuffer(VertexBuffer&& other) noexcept;
    VertexBuffer& operator=(VertexBuffer&& other) noexcept;
    
    /**
     * @brief Binds this buffer for rendering or modification
     */
    void bind() const;
    
    /**
     * @brief Unbinds this buffer
     */
    void unbind() const;
    
    /**
     * @brief Updates buffer data with new content
     * @param data Pointer to the new data
     * @param size Size of the data in bytes
     */
    void setData(const void* data, unsigned int size);
    
    /**
     * @brief Get the OpenGL buffer ID
     * @return The internal OpenGL buffer ID
     */
    GLuint getID() const { return mVBO; }

private:
    GLuint mVBO; ///< OpenGL buffer ID
};