#include "pch.hpp"
#include "OpenGLAPI.hpp"
#include "platform.hpp"
#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
OpenGLAPI::OpenGLAPI()
{
}

OpenGLAPI::~OpenGLAPI()
{
}

void OpenGLAPI::init()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
}

void OpenGLAPI::setViewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}

void OpenGLAPI::setClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void OpenGLAPI::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

