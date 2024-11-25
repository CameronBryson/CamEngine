#pragma once
#include "Engine/Util/platform.hpp"
class OpenGLContext
{
public:
	OpenGLContext(GLFWwindow* window);
	~OpenGLContext();

	void init();
	void swapBuffers();
private:
	GLFWwindow* window;
};