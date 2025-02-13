#include "pch.hpp"
#include "FrameBuffer.hpp"
#include "Engine/Platform/OpenGL/OpenGLFrameBuffer.hpp"

std::shared_ptr<FrameBuffer> FrameBuffer::createFrameBuffer(int width, int height, const std::vector<FrameBufferAttachmentSpecification>& attachments, int samples)
{
	return std::make_shared<OpenGLFrameBuffer>(width, height, attachments, samples);
}

