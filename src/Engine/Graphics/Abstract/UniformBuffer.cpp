#include "pch.hpp"
#include "UniformBuffer.hpp"
#include "Engine/Platform/OpenGL/OpenGLUniformBuffer.hpp"

std::shared_ptr<UniformBuffer> UniformBuffer::createUniformBuffer(unsigned int size, unsigned int binding)
{
    return std::make_shared<OpenGLUniformBuffer>(size, binding);
}
