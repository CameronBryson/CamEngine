#include "pch.hpp"
#include "EnvironmentMap.hpp"

#include "Texture.hpp"
#include "Shader.hpp"
#include "TextureSlots.hpp"
#include "OpenGLUtil.hpp"

#include <FrameBuffer.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
#include <stb_image.h>
#include <memory>


static glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 10.f);
static glm::mat4 captureViews[] =
{
	// +X face
	glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f),  glm::vec3(0.0f, -1.0f,  0.0f)),
	// -X face
	glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f),  glm::vec3(0.0f, -1.0f,  0.0f)),
	// +Y face (Top)
	glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec3(0.0f,  0.0f,  1.0f)),
	// -Y face (Bottom)
	glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec3(0.0f,  0.0f, -1.0f)),
	// +Z face
	glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f),  glm::vec3(0.0f, -1.0f,  0.0f)),
	// -Z face
	glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f),  glm::vec3(0.0f, -1.0f,  0.0f))
};

// Implementation of the OpenGL wrapper method
void EnvironmentMap::setGLDepthFunc(unsigned int func) {
    GL_CHECK(glDepthFunc(func));
}

EnvironmentMap::EnvironmentMap(const std::string& hdrPath,
										   std::shared_ptr<Shader> equirectangularToCubemapShader,
										   std::shared_ptr<Shader> irradianceShader,
										   std::shared_ptr<Shader> prefilterShader,
										   std::shared_ptr<Shader> brdfShader)
	: mEquirectangularToCubemapShader(equirectangularToCubemapShader)
	, mIrradianceShader(irradianceShader)
	, mPrefilterShader(prefilterShader)
	, mBRDFShader(brdfShader)
{
	GLint oldViewport[4];
	GL_CHECK(glGetIntegerv(GL_VIEWPORT, oldViewport));

	// Create cubemap from HDR environment map using the equirectangular projection shader
	mSkyboxCubemap = std::make_shared<Texture>(hdrPath, mEquirectangularToCubemapShader);

	generateIrradianceMap();
	generatePrefilterMap();
	generateBRDFLUT();

	GL_CHECK(glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]));
}

EnvironmentMap::~EnvironmentMap()
{
}


void EnvironmentMap::generateIrradianceMap()
{
	const unsigned int irradianceSize = 32;

	// Create empty cubemap using Texture's static helper method
	GLuint irradianceCubemapID = Texture::createEmptyCubemap(irradianceSize, Texture::Format::RGB16F);
	
	// Create Texture object to handle the cubemap
	mIrradianceCubemap = std::make_shared<Texture>(irradianceCubemapID, irradianceSize, irradianceSize, Texture::Type::CUBEMAP);
	
	// Set appropriate texture parameters using Texture's methods
	mIrradianceCubemap->setWrapMode(Texture::WrapMode::ClampToEdge, Texture::WrapMode::ClampToEdge, Texture::WrapMode::ClampToEdge);
	mIrradianceCubemap->setFilterMode(Texture::FilterMode::Linear, Texture::FilterMode::Linear);

	auto fbo = std::make_shared<FrameBuffer>(
		irradianceSize,
		irradianceSize,
		std::vector<FrameBufferAttachmentSpecification>{
		FrameBufferAttachmentSpecification(FrameBufferAttachmentType::Depth, FrameBufferTextureFormat::Depth24)
	}
	);
	fbo->bind();

	mIrradianceShader->use();
	mIrradianceShader->setMat4("projection", captureProjection);
	mIrradianceShader->setInt("environmentMap", IBLSlots::IRRADIANCE);
	mSkyboxCubemap->bind(IBLSlots::IRRADIANCE);

	GL_CHECK(glViewport(0, 0, irradianceSize, irradianceSize));

	for (unsigned int i = 0; i < 6; ++i)
	{
		mIrradianceShader->setMat4("view", captureViews[i]);

		fbo->attachExternalTexture(
			GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			irradianceCubemapID,
			0
		);
		fbo->setDrawBuffers({ GL_COLOR_ATTACHMENT0 });

		fbo->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gl::drawCube();
	}

	fbo->unbind();
}

void EnvironmentMap::generatePrefilterMap()
{
	const unsigned int prefilterSize = 128;

	// Create empty cubemap using Texture's static helper method
	GLuint prefilterCubemapID = Texture::createEmptyCubemap(prefilterSize, Texture::Format::RGB16F);
	
	// Create Texture object to handle the cubemap
	mPrefilterCubemap = std::make_shared<Texture>(prefilterCubemapID, prefilterSize, prefilterSize, Texture::Type::CUBEMAP);
	
	// Set appropriate texture parameters using Texture's methods
	mPrefilterCubemap->setWrapMode(Texture::WrapMode::ClampToEdge, Texture::WrapMode::ClampToEdge, Texture::WrapMode::ClampToEdge);
	mPrefilterCubemap->setFilterMode(Texture::FilterMode::LinearMipmapLinear, Texture::FilterMode::Linear);
	
	// Generate mipmaps
	mPrefilterCubemap->generateMipmaps();

	auto fbo = std::make_shared<FrameBuffer>(
		prefilterSize,
		prefilterSize,
		std::vector<FrameBufferAttachmentSpecification>{
		FrameBufferAttachmentSpecification(FrameBufferAttachmentType::Depth, FrameBufferTextureFormat::Depth24)
	}
	);

	mPrefilterShader->use();
	mPrefilterShader->setMat4("projection", captureProjection);
	mPrefilterShader->setInt("environmentMap", IBLSlots::PREFILTER);
	mSkyboxCubemap->bind(IBLSlots::PREFILTER);

	const unsigned int maxMipLevels = 5;

	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		unsigned int mipWidth = (unsigned int)(prefilterSize * std::pow(0.5, mip));
		unsigned int mipHeight = mipWidth;
		fbo->resize(mipWidth, mipHeight);

		GL_CHECK(glViewport(0, 0, mipWidth, mipHeight));
		fbo->bind();

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		mPrefilterShader->setFloat("roughness", roughness);

		for (unsigned int i = 0; i < 6; ++i)
		{
			mPrefilterShader->setMat4("view", captureViews[i]);

			fbo->attachExternalTexture(
				GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				prefilterCubemapID,
				mip
			);
			fbo->setDrawBuffers({ GL_COLOR_ATTACHMENT0 });

			fbo->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			gl::drawCube();
		}

		fbo->unbind();
	}
}

void EnvironmentMap::generateBRDFLUT()
{
	const unsigned int brdfLUTSize = 512;

	// Create empty 2D texture using Texture's static helper method
	GLuint brdfLUTID = Texture::createEmptyTexture2D(brdfLUTSize, brdfLUTSize, Texture::Format::RG);
	
	// Use Texture wrapper to bind and set parameters
	mBRDFLUT = std::make_shared<Texture>(brdfLUTID, brdfLUTSize, brdfLUTSize, Texture::Type::TEXTURE_2D);
	
	// Set appropriate texture parameters using Texture's methods
	mBRDFLUT->setWrapMode(Texture::WrapMode::ClampToEdge, Texture::WrapMode::ClampToEdge);
	mBRDFLUT->setFilterMode(Texture::FilterMode::Linear, Texture::FilterMode::Linear);

	auto fbo = std::make_shared<FrameBuffer>(
		brdfLUTSize,
		brdfLUTSize,
		std::vector<FrameBufferAttachmentSpecification>{
		FrameBufferAttachmentSpecification(FrameBufferAttachmentType::Color, FrameBufferTextureFormat::RG16F)
	}
	);
	fbo->bind();
	GL_CHECK(glViewport(0, 0, brdfLUTSize, brdfLUTSize));

	fbo->attachExternalTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTID, 0);
	fbo->setDrawBuffers({ GL_COLOR_ATTACHMENT0 });

	fbo->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mBRDFShader->use();
	gl::drawQuad();

	fbo->unbind();
}

void EnvironmentMap::bindIrradiance(int slot)
{
	mIrradianceCubemap->bind(slot);
}

void EnvironmentMap::bindPrefilter(int slot)
{

	mPrefilterCubemap->bind(slot);
}

void EnvironmentMap::bindBRDFLUT(int slot)
{
	mBRDFLUT->bind(slot);
}

void EnvironmentMap::unbindIrradiance(int slot)
{
	mIrradianceCubemap->unbind(slot);
}

void EnvironmentMap::unbindPrefilter(int slot)
{
	mPrefilterCubemap->unbind(slot);
}

void EnvironmentMap::unbindBRDFLUT(int slot)
{
	mBRDFLUT->unbind(slot);
}

void EnvironmentMap::drawSkybox(std::shared_ptr<Shader>& skyboxShader)
{
	setGLDepthFunc(GL_LEQUAL);
	skyboxShader->use();
	skyboxShader->setInt("skybox", IBLSlots::SKYBOX);
	mSkyboxCubemap->bind(IBLSlots::SKYBOX);
	gl::drawCube();
	setGLDepthFunc(GL_LESS);
}
