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
	               const std::shared_ptr<Shader>& equirectangularToCubemapShader,
				   const std::shared_ptr<Shader>& irradianceShader,
				   const std::shared_ptr<Shader>& prefilterShader,
				   const std::shared_ptr<Shader>& brdfShader);
	~EnvironmentMap();

	// Generate different maps for PBR rendering
	void generateIrradianceMap(const std::shared_ptr<Shader>& irradianceShader);
	void generatePrefilterMap(const std::shared_ptr<Shader>& prefilterShader);
	void generateBRDFLUT(const std::shared_ptr<Shader>& brdfShader);

	// Texture binding utilities
	void bindIrradiance(int slot) const;
	void bindPrefilter(int slot) const;
	void bindBRDFLUT(int slot) const;
	void unbindIrradiance(int slot) const;
	void unbindPrefilter(int slot) const;
	void unbindBRDFLUT(int slot) const;

	bool isLoaded() const
	{
		return mSkyboxCubemap && mIrradianceCubemap && mPrefilterCubemap && mBRDFLUT;
	}
	// Draw the skybox
	void drawSkybox(const std::shared_ptr<Shader>& skyboxShader) const;

private:

	std::unique_ptr<Texture> mSkyboxCubemap;
	std::unique_ptr<Texture> mIrradianceCubemap;
	std::unique_ptr<Texture> mPrefilterCubemap;
	std::unique_ptr<Texture> mBRDFLUT;
};

