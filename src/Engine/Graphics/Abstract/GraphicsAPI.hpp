#pragma once
#include <memory>
class VertexArray;
class GraphicsAPI
{
public:
	virtual ~GraphicsAPI() = default;
	virtual void init() = 0;
	virtual void setViewport(int x, int y, int width, int height) = 0;
	virtual void clear() = 0;
	virtual void setClearColor(float r, float g, float b, float a) = 0;

	static std::shared_ptr<GraphicsAPI> createGraphicsAPI();
};