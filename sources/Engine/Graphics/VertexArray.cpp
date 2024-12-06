#include "Engine/pch.hpp"
#include "VertexArray.hpp"
#include "Engine/Platform/OpenGL/OpenGLVertexArray.hpp"

std::shared_ptr<VertexArray> VertexArray::create()
{
	return std::make_shared<OpenGLVertexArray>();
}
