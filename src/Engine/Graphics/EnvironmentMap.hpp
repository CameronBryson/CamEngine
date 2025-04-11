#pragma once
#include <memory>
#include "Texture.hpp"
#include <Mesh.hpp>
#include "Shader.hpp"
#include <VertexArray.hpp>

class EnvironmentMap
{
public:
	EnvironmentMap(const std::string& hdrPath, std::shared_ptr<Shader> equirectangularToCubemapShader, std::shared_ptr<Shader> irradianceShader, std::shared_ptr<Shader> prefilterShader, std::shared_ptr<Shader> brdfShader);
	~EnvironmentMap();

	void generateIrradianceMap();
	void generatePrefilterMap();
	void generateBRDFLUT();

	void bindIrradiance(int slot);
	void bindPrefilter(int slot);
	void bindBRDFLUT(int slot);
	void unbindIrradiance(int slot);
	void unbindPrefilter(int slot);
	void unbindBRDFLUT(int slot);

	void drawSkybox(std::shared_ptr<Shader>& skyboxShader);

protected:
	// OpenGL wrapper methods to minimize direct OpenGL calls
	static void setGLDepthFunc(unsigned int func);
	
private:
	std::shared_ptr<Texture> mSkyboxCubemap;
	std::shared_ptr<Texture> mIrradianceCubemap;
	std::shared_ptr<Texture> mPrefilterCubemap;
	std::shared_ptr<Texture> mBRDFLUT;

	std::shared_ptr<Shader> mEquirectangularToCubemapShader;
	std::shared_ptr<Shader> mIrradianceShader;
	std::shared_ptr<Shader> mPrefilterShader;
	std::shared_ptr<Shader> mBRDFShader;
};

