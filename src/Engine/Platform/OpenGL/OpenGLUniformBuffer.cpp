#include "pch.hpp"
#include "OpenGLUniformBuffer.hpp"
#include "platform.hpp"
OpenGLUniformBuffer::OpenGLUniformBuffer(unsigned int size, unsigned int binding)
{
	glCreateBuffers(1, &mRendererID);
}

OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
}

void OpenGLUniformBuffer::setData(const void* data, unsigned int size, unsigned int offset)
{

}
