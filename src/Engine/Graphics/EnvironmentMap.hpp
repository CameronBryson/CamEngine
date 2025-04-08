#pragma once
#include <memory>
#include <TextureCubemap.hpp>
#include <Texture2D.hpp>
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

private:
	std::shared_ptr<TextureCubemap> mSkyboxCubemap;
	std::shared_ptr<TextureCubemap> mIrradianceCubemap;
	std::shared_ptr<TextureCubemap> mPrefilterCubemap;
	std::shared_ptr<Texture2D>      mBRDFLUT;

	std::shared_ptr<Shader> mEquirectangularToCubemapShader;
	std::shared_ptr<Shader> mIrradianceShader;
	std::shared_ptr<Shader> mPrefilterShader;
	std::shared_ptr<Shader> mBRDFShader;


};

