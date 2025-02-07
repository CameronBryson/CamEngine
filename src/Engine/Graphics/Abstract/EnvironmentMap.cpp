#include "pch.hpp"
#include "EnvironmentMap.hpp"
#include "OpenGLEnvironmentMap.hpp"


std::shared_ptr<EnvironmentMap> EnvironmentMap::createEnvironmentMap(const std::string& hdrPath, std::shared_ptr<Shader> equirectangularToCubemapShader, std::shared_ptr<Shader> irradianceShader, std::shared_ptr<Shader> prefilterShader, std::shared_ptr<Shader> brdfShader)
{
	return std::make_shared<OpenGLEnvironmentMap>(hdrPath, equirectangularToCubemapShader, irradianceShader, prefilterShader,brdfShader);
}
