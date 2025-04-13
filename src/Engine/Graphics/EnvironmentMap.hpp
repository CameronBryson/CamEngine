#pragma once
#include <memory>
// #include "Texture.hpp" // Forward declare instead
// #include <Mesh.hpp> // Unused
// #include "Shader.hpp" // Forward declare instead
// #include <VertexArray.hpp> // Unused
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
	void generateIrradianceMap();
	void generatePrefilterMap();
	void generateBRDFLUT();

	// Texture binding utilities
	void bindIrradiance(int slot);
	void bindPrefilter(int slot);
	void bindBRDFLUT(int slot);
	void unbindIrradiance(int slot);
	void unbindPrefilter(int slot);
	void unbindBRDFLUT(int slot);

	// Draw the skybox
	void drawSkybox(std::shared_ptr<Shader>& skyboxShader);

private:
	// OpenGL wrapper methods - Keep static
	static void setGLDepthFunc(unsigned int func);
	
	std::shared_ptr<Texture> mSkyboxCubemap;
	std::shared_ptr<Texture> mIrradianceCubemap;
	std::shared_ptr<Texture> mPrefilterCubemap;
	std::shared_ptr<Texture> mBRDFLUT;

	std::shared_ptr<Shader> mEquirectangularToCubemapShader;
	std::shared_ptr<Shader> mIrradianceShader;
	std::shared_ptr<Shader> mPrefilterShader;
	std::shared_ptr<Shader> mBRDFShader;
};

