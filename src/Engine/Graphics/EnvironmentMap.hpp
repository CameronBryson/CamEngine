#pragma once
#include <memory>
#include <string_view>

// Forward declarations
class Texture;
class Shader;

/**
 * Manages environment mapping for image-based lighting (IBL)
 * Handles skybox, irradiance, prefiltered environment maps and BRDF LUT
 */
class EnvironmentMap
{
public:
	// Create an environment map from an HDR equirectangular map
	EnvironmentMap(std::string_view hdrPath, 
	              std::shared_ptr<Shader> equirectangularToCubemapShader, 
	              std::shared_ptr<Shader> irradianceShader, 
	              std::shared_ptr<Shader> prefilterShader, 
	              std::shared_ptr<Shader> brdfShader);
	~EnvironmentMap();

	// Generate different maps for PBR rendering
	void generateIrradianceMap(std::shared_ptr<Shader> irradianceShader);
	void generatePrefilterMap(std::shared_ptr<Shader> prefilterShader);
	void generateBRDFLUT(std::shared_ptr<Shader> brdfShader);

	// Texture binding utilities
	void bindIrradiance(int slot);
	void bindPrefilter(int slot);
	void bindBRDFLUT(int slot);
	void unbindIrradiance(int slot);
	void unbindPrefilter(int slot);
	void unbindBRDFLUT(int slot);

	bool isLoaded() const
	{
		return mSkyboxCubemap && mIrradianceCubemap && mPrefilterCubemap && mBRDFLUT;
	}
	// Draw the skybox
	void drawSkybox(std::shared_ptr<Shader>& skyboxShader);

private:
	
	std::unique_ptr<Texture> mSkyboxCubemap;
	std::unique_ptr<Texture> mIrradianceCubemap;
	std::unique_ptr<Texture> mPrefilterCubemap;
	std::unique_ptr<Texture> mBRDFLUT;
};

