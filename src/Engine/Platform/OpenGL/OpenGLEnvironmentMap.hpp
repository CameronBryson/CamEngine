#pragma once
#include "EnvironmentMap.hpp"
#include <memory>
#include <TextureCubemap.hpp>
#include <Texture2D.hpp>
#include <Mesh.hpp>
#include "Shader.hpp"
#include <VertexArray.hpp>
class OpenGLEnvironmentMap : public EnvironmentMap
{
public:
	OpenGLEnvironmentMap(const std::string& hdrPath, std::shared_ptr<Shader> equirectangularToCubemapShader, std::shared_ptr<Shader> irradianceShader, std::shared_ptr<Shader> prefilterShader, std::shared_ptr<Shader> brdfShader);
    ~OpenGLEnvironmentMap() override;

    void generateIrradianceMap();
    void generatePrefilterMap();
	void generateBRDFLUT();

    void bindIrradiance(int slot) override;
    void bindPrefilter(int slot) override;
    void bindBRDFLUT(int slot) override;
	void unbindIrradiance(int slot) override;
	void unbindPrefilter(int slot) override;
	void unbindBRDFLUT(int slot) override;

    // Draw a skybox using the environment map
    void drawSkybox(std::shared_ptr<Shader>& skyboxShader) override;

private:
    std::shared_ptr<TextureCubemap> mSkyboxCubemap;     // Equirect->Cubemap
    std::shared_ptr<TextureCubemap> mIrradianceCubemap; // Convolved irradiance
    std::shared_ptr<TextureCubemap> mPrefilterCubemap;  // Prefiltered env
    std::shared_ptr<Texture2D>      mBRDFLUT;

	std::shared_ptr<Shader> mEquirectangularToCubemapShader;
	std::shared_ptr<Shader> mIrradianceShader;
	std::shared_ptr<Shader> mPrefilterShader;
	std::shared_ptr<Shader> mBRDFShader;


};

