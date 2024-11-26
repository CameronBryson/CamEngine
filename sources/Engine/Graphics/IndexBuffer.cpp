#include "IndexBuffer.hpp"
#include "Engine/Platform/OpenGL/OpenGLIndexBuffer.hpp"
std::shared_ptr<IndexBuffer> IndexBuffer::create(const std::vector<unsigned>& indices)
{
	return std::make_shared<OpenGLIndexBuffer>(indices);
}