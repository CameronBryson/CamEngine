#pragma once
#include <memory>
class GraphicsContext 
{
public:
	virtual ~GraphicsContext() = default;

	virtual void init() = 0;
	virtual void swapBuffers() = 0;

	static std::shared_ptr<GraphicsContext> createGraphicsContext(void* window);
};