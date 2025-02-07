#include "pch.hpp"
#include "VertexBuffer.hpp"
#include "Engine/Platform/OpenGL/OpenGLVertexBuffer.hpp"

#include <memory>

std::shared_ptr<VertexBuffer> VertexBuffer::create(const std::vector<Vertex>& vertices)
{
	return std::make_shared<OpenGLVertexBuffer>(vertices);
}
