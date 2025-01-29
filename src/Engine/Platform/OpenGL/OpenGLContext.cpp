#include "pch.hpp"
#include "OpenGLContext.hpp"

OpenGLContext::OpenGLContext(GLFWwindow* window) : mWindow(window)
{
}

OpenGLContext::~OpenGLContext()
{
}

void OpenGLContext::init()
{
}

void OpenGLContext::swapBuffers()
{
	glfwSwapBuffers(mWindow);
}
