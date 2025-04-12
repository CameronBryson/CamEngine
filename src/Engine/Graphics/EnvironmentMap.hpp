#pragma once
#include <memory>
#include "Texture.hpp"
#include <Mesh.hpp>
#include "Shader.hpp"
#include <VertexArray.hpp>
#include <string_view>
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

protected:
	// OpenGL wrapper methods
	static void setGLDepthFunc(unsigned int func);
	
private:
	//Most of this stuff we dont need to store here
	std::shared_ptr<Texture> mSkyboxCubemap;
	std::shared_ptr<Texture> mIrradianceCubemap;
	std::shared_ptr<Texture> mPrefilterCubemap;
	std::shared_ptr<Texture> mBRDFLUT;

	std::shared_ptr<Shader> mEquirectangularToCubemapShader;
	std::shared_ptr<Shader> mIrradianceShader;
	std::shared_ptr<Shader> mPrefilterShader;
	std::shared_ptr<Shader> mBRDFShader;
};

