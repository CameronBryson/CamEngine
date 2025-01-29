#pragma once
#include "GraphicsContext.hpp"
#include "platform.hpp"
class OpenGLContext : public GraphicsContext
{
public:
	OpenGLContext(GLFWwindow* window);
	~OpenGLContext();
	void init() override;
	void swapBuffers() override;
private:
	GLFWwindow* mWindow;
	
};

