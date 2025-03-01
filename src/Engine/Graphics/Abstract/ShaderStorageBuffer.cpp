#include "pch.hpp"
#include "ShaderStorageBuffer.hpp"
#include "OpenGLShaderStorageBuffer.hpp"

std::shared_ptr<ShaderStorageBuffer> ShaderStorageBuffer::create(unsigned int size, unsigned int binding)
{
	return std::make_shared<OpenGLShaderStorageBuffer>(size, binding);
}
