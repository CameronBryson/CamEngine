
#include "pch.hpp"
#include "GraphicsAPI.hpp"
#include "OpenGLAPI.hpp"
std::shared_ptr<GraphicsAPI> GraphicsAPI::createGraphicsAPI()
{
	return std::make_shared<OpenGLAPI>();
}
