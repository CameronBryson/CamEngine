#include "pch.hpp"
#include "GraphicsAPI.hpp"
#include "platform.hpp"
#include "VertexArray.hpp"
#include "IndexBuffer.hpp"
GraphicsAPI::GraphicsAPI()
{
}

GraphicsAPI::~GraphicsAPI()
{
}

void GraphicsAPI::init()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
}

void GraphicsAPI::setViewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}

void GraphicsAPI::setClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void GraphicsAPI::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

