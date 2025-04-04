#pragma once
#include "Engine/Util/platform.hpp"
#include <vector>
#include <Vertex.hpp>
class VertexBuffer
{
public:
	VertexBuffer(const std::vector<Vertex>& vertices);
	~VertexBuffer();
	void bind() const;
	void unbind() const;
	void setData(const void* data, unsigned int size);
private:
	GLuint mVBO;
};