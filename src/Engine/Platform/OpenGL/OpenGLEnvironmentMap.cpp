#include "pch.hpp"
#include "OpenGLEnvironmentMap.hpp"
#include "OpenGLTextureCubemap.hpp"
#include "GraphicsManager.hpp"
#include "Shader.hpp"
#include <MeshFactory.hpp>
#include "TextureSlots.hpp"


OpenGLEnvironmentMap::OpenGLEnvironmentMap(const std::string& hdrPath, std::shared_ptr<Shader> equirectangularToCubemapShader, std::shared_ptr<Shader> irradianceShader, std::shared_ptr<Shader> prefilterShader, std::shared_ptr<Shader> brdfShader) : mEquirectangularToCubemapShader(equirectangularToCubemapShader), mIrradianceShader(irradianceShader), mPrefilterShader(prefilterShader), mBRDFShader(brdfShader)
{
	// Load the environment cubemap ( skybox )
	mSkyboxCubemap = OpenGLTextureCubemap::createTextureCubemap(hdrPath, equirectangularToCubemapShader);
	// Initialize the skybox mesh
	mQuadMesh = MeshFactory::createQuad();
	mCubeMesh = MeshFactory::createCube();
}

void OpenGLEnvironmentMap::generateIrradianceMap()
{
	const unsigned int irradianceMapSize = 32;

}

void OpenGLEnvironmentMap::generatePrefilterMap()
{
	const unsigned int prefilterMapSize = 128;
}

void OpenGLEnvironmentMap::generateBRDFLUT()
{
	const unsigned int BRDFLUTSize = 512;
}

void OpenGLEnvironmentMap::bindIrradiance(int slot)
{
	mIrradianceCubemap->bind(slot);
	mIrradianceShader->setInt("irradianceMap", slot);
}

void OpenGLEnvironmentMap::bindPrefilter(int slot)
{
	mPrefilterCubemap->bind(slot);
	mPrefilterShader->setInt("prefilterMap", slot);
}

void OpenGLEnvironmentMap::bindBRDFLUT(int slot)
{
	mBRDFLUT->bind(slot);
	mBRDFShader->setInt("brdfLUT", slot);
}
void OpenGLEnvironmentMap::drawSkybox(std::shared_ptr<Shader>& skyboxShader)
{
	glDepthFunc(GL_LEQUAL);
	//draw
	glDepthFunc(GL_LESS);
}
