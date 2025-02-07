#include "pch.hpp"
#include "FrameBuffer.hpp"
#include "Engine/Platform/OpenGL/OpenGLFrameBuffer.hpp"

std::shared_ptr<FrameBuffer> FrameBuffer::createFrameBuffer(int width, int height)
{
    return std::make_shared<OpenGLFrameBuffer>(width, height);
}
