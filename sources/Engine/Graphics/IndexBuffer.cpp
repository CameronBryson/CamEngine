#include "IndexBuffer.hpp"
#include "Engine/Platform/OpenGL/OpenGLIndexBuffer.hpp"
std::shared_ptr<IndexBuffer> IndexBuffer::create(unsigned int* indices, unsigned int count)
{
	return std::make_shared<OpenGLIndexBuffer>(indices, count);
}