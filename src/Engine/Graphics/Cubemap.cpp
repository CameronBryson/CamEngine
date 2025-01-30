#include "pch.hpp"
#include "Cubemap.hpp"
#include "OpenGLCubemap.hpp"

std::shared_ptr<Cubemap> Cubemap::createCubemap(const std::string& path)
{
	return std::make_shared<OpenGLCubemap>(path);
}
