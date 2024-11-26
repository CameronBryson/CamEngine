#pragma once
#include "Engine/Graphics/Vertex.hpp"
#include <vector>
#include <memory>
class VertexBuffer
{
public:
	virtual ~VertexBuffer() = default;

	virtual void bind() const = 0;
	virtual void unbind() const = 0;

	virtual void setData(const void* data, unsigned int size) = 0;

	static std::shared_ptr<VertexBuffer> create(const std::vector<Vertex>& vertices);

};