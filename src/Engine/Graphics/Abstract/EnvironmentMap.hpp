#pragma once
#include <string>
#include "Shader.hpp"
#include "TextureSlots.hpp"

class EnvironmentMap
{
public:
    virtual ~EnvironmentMap() = default;


    // Bind each resource so PBR shaders can sample them
    virtual void bindIrradiance(int slot = IBLSlots::IRRADIANCE) = 0;
    virtual void bindPrefilter(int slot = IBLSlots::PREFILTER) = 0;
    virtual void bindBRDFLUT(int slot = IBLSlots::BRDFLUT) = 0;
	virtual void unbindIrradiance(int slot = IBLSlots::IRRADIANCE) = 0;
	virtual void unbindPrefilter(int slot = IBLSlots::PREFILTER) = 0;
	virtual void unbindBRDFLUT(int slot = IBLSlots::BRDFLUT) = 0;

    // Draw a skybox using the environment map
    virtual void drawSkybox(std::shared_ptr<Shader>& skyboxShader) = 0;

    static std::shared_ptr<EnvironmentMap> createEnvironmentMap(const std::string& hdrPath,  std::shared_ptr<Shader> equirectangularToCubemapShader, std::shared_ptr<Shader> irradianceShader, std::shared_ptr<Shader> prefilterShader, std::shared_ptr<Shader> brdfShader);
};
