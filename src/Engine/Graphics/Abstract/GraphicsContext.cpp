#include "pch.hpp"
#include "GraphicsContext.hpp"
#include "OpenGLContext.hpp"
#include "platform.hpp"
std::shared_ptr<GraphicsContext> GraphicsContext::createGraphicsContext(void* window)
{
	return std::make_shared<OpenGLContext>(static_cast<GLFWwindow*>(window));
}
