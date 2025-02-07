#pragma once
#include "GraphicsAPI.hpp"
class OpenGLAPI : public GraphicsAPI
{
public:
	OpenGLAPI();
	~OpenGLAPI();
	void init() override;
	void setViewport(int x, int y, int width, int height) override;
	void setClearColor(float r, float g, float b, float a) override;
	void clear() override;

private:
};