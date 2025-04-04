#include "pch.hpp"
#include "GraphicsContext.hpp"

GraphicsContext::GraphicsContext(GLFWwindow* window) : mWindow(window)
{
}

GraphicsContext::~GraphicsContext()
{
}

void GraphicsContext::init()
{
}

void GraphicsContext::swapBuffers()
{
	glfwSwapBuffers(mWindow);
}
