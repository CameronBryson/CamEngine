#include "pch.hpp"
#include "EnvironmentMap.hpp"

#include "Texture.hpp"
#include "Shader.hpp"
#include "TextureSlots.hpp"
#include "OpenGLUtil.hpp"
#include "FrameBuffer.hpp"
#include "Engine/Util/Logging.hpp"
#include "Engine/Util/ErrorHandler.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
#include <stb_image.h>
#include <memory>

namespace {
    // Constants for generated map resolutions and mip levels
    constexpr unsigned int IRRADIANCE_MAP_SIZE = 32;
    constexpr unsigned int PREFILTER_MAP_SIZE = 128;
    constexpr unsigned int PREFILTER_MAX_MIP_LEVELS = 5;
    constexpr unsigned int BRDF_LUT_SIZE = 512;
} // anonymous namespace

// Static view matrices for cubemap faces (constant for all cubemap operations)
static const glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 10.f);
static const glm::mat4 captureViews[] =
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

EnvironmentMap::EnvironmentMap(std::string_view hdrPath,
							  std::shared_ptr<Shader> equirectangularToCubemapShader,
							  std::shared_ptr<Shader> irradianceShader,
							  std::shared_ptr<Shader> prefilterShader,
							  std::shared_ptr<Shader> brdfShader)
{
    LOG_INFO(logging::gGraphicsLogger, "Creating EnvironmentMap from HDR: {}", hdrPath);
    // --- Error Checking for Shaders ---
    if (!equirectangularToCubemapShader) {
        throw error_handling::GraphicsException("EnvironmentMap requires a valid EquirectangularToCubemap shader.");
    }
    if (!irradianceShader) {
        throw error_handling::GraphicsException("EnvironmentMap requires a valid Irradiance shader.");
    }
    if (!prefilterShader) {
        throw error_handling::GraphicsException("EnvironmentMap requires a valid Prefilter shader.");
    }
    if (!brdfShader) {
        throw error_handling::GraphicsException("EnvironmentMap requires a valid BRDF shader.");
    }

	// Save current viewport to restore it later
	GLint oldViewport[4];
	GL_CHECK(glGetIntegerv(GL_VIEWPORT, oldViewport));

	// Create cubemap from HDR environment map
	mSkyboxCubemap = std::make_shared<Texture>(hdrPath, equirectangularToCubemapShader);

	// Generate all PBR related maps
	generateIrradianceMap(irradianceShader);
	generatePrefilterMap(prefilterShader);
	generateBRDFLUT(brdfShader);

	// Restore original viewport
	GL_CHECK(glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]));
}

EnvironmentMap::~EnvironmentMap()
{
	// Resources are automatically cleaned up by shared_ptr
}

void EnvironmentMap::generateIrradianceMap(std::shared_ptr<Shader> irradianceShader)
{
	LOG_DEBUG(logging::gGraphicsLogger, "Generating Irradiance Map ({}x{})...", IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE);

    if (!irradianceShader) {
        LOG_ERROR(logging::gGraphicsLogger, "Cannot generate Irradiance Map: Irradiance shader is missing.");
        return;
    }
    if (!mSkyboxCubemap) {
        LOG_ERROR(logging::gGraphicsLogger, "Cannot generate Irradiance Map: Skybox cubemap is missing.");
        return;
    }

	// Create empty cubemap and texture object
	GLuint irradianceCubemapID = Texture::createEmptyCubemap(IRRADIANCE_MAP_SIZE, Texture::Format::RGB16F);
	mIrradianceCubemap = std::make_shared<Texture>(irradianceCubemapID, IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE, Texture::Type::CUBEMAP);
	
	// Set texture parameters
	mIrradianceCubemap->setWrapMode(Texture::WrapMode::ClampToEdge, Texture::WrapMode::ClampToEdge, Texture::WrapMode::ClampToEdge);
	mIrradianceCubemap->setFilterMode(Texture::FilterMode::Linear, Texture::FilterMode::Linear);

	// Create framebuffer for irradiance map rendering
	auto fbo = std::make_unique<FrameBuffer>(
		IRRADIANCE_MAP_SIZE,
		IRRADIANCE_MAP_SIZE,
		std::vector<FrameBufferAttachmentSpecification>{
		    FrameBufferAttachmentSpecification(FrameBufferAttachmentType::Color, FrameBufferTextureFormat::RGBA16F, "IrradianceColor")
	    },
        1, // No multisampling needed for generation
        "IrradianceFBO"
	);
	
	// Setup for irradiance convolution
	fbo->bind();
	irradianceShader->use();
	irradianceShader->setMat4("projection", captureProjection);
	irradianceShader->setInt("environmentMap", IBLSlots::IRRADIANCE);
	mSkyboxCubemap->bind(IBLSlots::IRRADIANCE);
	GL_CHECK(glViewport(0, 0, IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE));

	// Render to all six cubemap faces
	for (unsigned int i = 0; i < 6; ++i)
	{
		irradianceShader->setMat4("view", captureViews[i]);
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

void EnvironmentMap::generatePrefilterMap(std::shared_ptr<Shader> prefilterShader)
{
	LOG_DEBUG(logging::gGraphicsLogger, "Generating Prefilter Map ({}x{}, {} mips)...", PREFILTER_MAP_SIZE, PREFILTER_MAP_SIZE, PREFILTER_MAX_MIP_LEVELS);

    if (!prefilterShader) {
        LOG_ERROR(logging::gGraphicsLogger, "Cannot generate Prefilter Map: Prefilter shader is missing.");
        return;
    }
    if (!mSkyboxCubemap) {
        LOG_ERROR(logging::gGraphicsLogger, "Cannot generate Prefilter Map: Skybox cubemap is missing.");
        return;
    }

	// Create empty cubemap with mipmaps
	GLuint prefilterCubemapID = Texture::createEmptyCubemap(PREFILTER_MAP_SIZE, Texture::Format::RGB16F);
	mPrefilterCubemap = std::make_shared<Texture>(prefilterCubemapID, PREFILTER_MAP_SIZE, PREFILTER_MAP_SIZE, Texture::Type::CUBEMAP);
	
	// Set parameters and generate mipmaps
	mPrefilterCubemap->setWrapMode(Texture::WrapMode::ClampToEdge, Texture::WrapMode::ClampToEdge, Texture::WrapMode::ClampToEdge);
	mPrefilterCubemap->setFilterMode(Texture::FilterMode::LinearMipmapLinear, Texture::FilterMode::Linear);
	mPrefilterCubemap->generateMipmaps();

	// Create framebuffer for prefilter map rendering
	auto fbo = std::make_unique<FrameBuffer>(
		PREFILTER_MAP_SIZE, // Initial size, will be resized per mip
		PREFILTER_MAP_SIZE,
		std::vector<FrameBufferAttachmentSpecification>{
		    FrameBufferAttachmentSpecification(FrameBufferAttachmentType::Color, FrameBufferTextureFormat::RGBA16F, "PrefilterColor")
	    },
        1, // No multisampling
        "PrefilterFBO"
	);

	// Setup shader uniforms
	prefilterShader->use();
	prefilterShader->setMat4("projection", captureProjection);
	prefilterShader->setInt("environmentMap", IBLSlots::PREFILTER);
	mSkyboxCubemap->bind(IBLSlots::PREFILTER);

	// Process each mip level
	for (unsigned int mip = 0; mip < PREFILTER_MAX_MIP_LEVELS; ++mip)
	{
		// Resize based on mip level
        // Use std::max to prevent 0x0 dimension for the last mip level if size isn't power of 2
		unsigned int mipWidth = std::max(1u, static_cast<unsigned int>(PREFILTER_MAP_SIZE * std::pow(0.5, mip)));
		unsigned int mipHeight = std::max(1u, static_cast<unsigned int>(PREFILTER_MAP_SIZE * std::pow(0.5, mip)));
		fbo->resize(mipWidth, mipHeight); // Framebuffer handles its own resizing
		GL_CHECK(glViewport(0, 0, mipWidth, mipHeight));
		
		// Calculate roughness for this mip level
		float roughness = (float)mip / (float)(PREFILTER_MAX_MIP_LEVELS - 1);
		prefilterShader->setFloat("roughness", roughness);
		
		fbo->bind();

		// Render to all six faces for this mip level
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader->setMat4("view", captureViews[i]);
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

void EnvironmentMap::generateBRDFLUT(std::shared_ptr<Shader> brdfShader)
{
	LOG_DEBUG(logging::gGraphicsLogger, "Generating BRDF LUT ({}x{})...", BRDF_LUT_SIZE, BRDF_LUT_SIZE);
    if (!brdfShader) {
        LOG_ERROR(logging::gGraphicsLogger, "Cannot generate BRDF LUT: BRDF shader is missing.");
        return;
    }

	// Create empty 2D texture for the BRDF lookup table
	GLuint brdfLUTID = Texture::createEmptyTexture2D(BRDF_LUT_SIZE, BRDF_LUT_SIZE, Texture::Format::RG);
	mBRDFLUT = std::make_shared<Texture>(brdfLUTID, BRDF_LUT_SIZE, BRDF_LUT_SIZE, Texture::Type::TEXTURE_2D);
	
	// Set appropriate texture parameters
	mBRDFLUT->setWrapMode(Texture::WrapMode::ClampToEdge, Texture::WrapMode::ClampToEdge);
	mBRDFLUT->setFilterMode(Texture::FilterMode::Linear, Texture::FilterMode::Linear);

	// Create framebuffer for BRDF LUT generation
	auto fbo = std::make_unique<FrameBuffer>(
		BRDF_LUT_SIZE,
		BRDF_LUT_SIZE,
		std::vector<FrameBufferAttachmentSpecification>{
		    FrameBufferAttachmentSpecification(FrameBufferAttachmentType::Color, FrameBufferTextureFormat::RG16F, "BRDFColor")
	    },
        1, // No multisampling
        "BRDFLUT_FBO"
	);
	
	// Render the BRDF integration map
	fbo->bind();
	GL_CHECK(glViewport(0, 0, BRDF_LUT_SIZE, BRDF_LUT_SIZE));
	fbo->attachExternalTexture(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTID, 0);
	fbo->setDrawBuffers({ GL_COLOR_ATTACHMENT0 });
	fbo->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	brdfShader->use();
	gl::drawQuad();
	fbo->unbind();
}

// Texture binding utilities
void EnvironmentMap::bindIrradiance(int slot)
{
    if (mIrradianceCubemap) mIrradianceCubemap->bind(slot);
    else LOG_WARN(logging::gGraphicsLogger, "Attempted to bind null Irradiance map.");
}

void EnvironmentMap::bindPrefilter(int slot)
{
    if (mPrefilterCubemap) mPrefilterCubemap->bind(slot);
    else LOG_WARN(logging::gGraphicsLogger, "Attempted to bind null Prefilter map.");
}

void EnvironmentMap::bindBRDFLUT(int slot)
{
    if (mBRDFLUT) mBRDFLUT->bind(slot);
    else LOG_WARN(logging::gGraphicsLogger, "Attempted to bind null BRDF LUT.");
}

void EnvironmentMap::unbindIrradiance(int slot)
{
    if (mIrradianceCubemap) mIrradianceCubemap->unbind(slot);
}

void EnvironmentMap::unbindPrefilter(int slot)
{
    if (mPrefilterCubemap) mPrefilterCubemap->unbind(slot);
}

void EnvironmentMap::unbindBRDFLUT(int slot)
{
    if (mBRDFLUT) mBRDFLUT->unbind(slot);
}

void EnvironmentMap::drawSkybox(std::shared_ptr<Shader>& skyboxShader)
{
    if (!skyboxShader) {
        LOG_ERROR(logging::gGraphicsLogger, "Attempted to draw skybox with null shader.");
        return;
    }
    if (!mSkyboxCubemap) {
        LOG_ERROR(logging::gGraphicsLogger, "Attempted to draw null skybox cubemap.");
        return;
    }
	setGLDepthFunc(GL_LEQUAL); // Ensure depth test passes when fragments are at maximum depth
	skyboxShader->use();
	skyboxShader->setInt("skybox", IBLSlots::SKYBOX); // Use defined slot
	mSkyboxCubemap->bind(IBLSlots::SKYBOX);
	gl::drawCube();
	mSkyboxCubemap->unbind(IBLSlots::SKYBOX); // Unbind after drawing
	setGLDepthFunc(GL_LESS); // Reset depth function to default
}
