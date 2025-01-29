#include "pch.hpp"
#include "Cubemap.hpp"
#include "OpenGLCubemap.hpp"

std::shared_ptr<Cubemap> Cubemap::createCubemap(const std::vector<std::string> facePaths)
{
	return std::make_shared<OpenGLCubemap>(facePaths);
}
