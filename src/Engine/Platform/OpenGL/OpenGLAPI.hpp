#pragma once
#include "Engine/Graphics/GraphicsAPI.hpp"
class OpenGLAPI : public GraphicsAPI
{
public:
	OpenGLAPI();
	~OpenGLAPI();
	void init() override;
	void setViewport(int x, int y, int width, int height) override;
	void setClearColor(float r, float g, float b, float a) override;
	void clear() override;

	void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
	void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray, unsigned int indexCount) override;
private:
};