#pragma once
#include "platform.hpp"
class GraphicsContext
{
public:
	GraphicsContext(GLFWwindow* window);
	~GraphicsContext();
	void init();
	void swapBuffers();
private:
	GLFWwindow* mWindow;

};

