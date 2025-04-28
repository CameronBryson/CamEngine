#include "pch.hpp"
#include "SRender.hpp"

#include "Engine/Components.hpp"
#include "Engine/Util/EngineUtil.hpp"
#include "Engine/Util/platform.hpp"

#include "BaseScene.hpp"
#include "Engine/Managers/GameManager.hpp"
#include "Engine/Managers/GraphicsManager.hpp"
#include "Engine/Util/OpenGLUtil.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Mesh.hpp"
#include "Model.hpp"
#include "platform.hpp"
#include "Shader.hpp"
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include <GameSettings.hpp>
#include <TextureSlots.hpp>
#include "UniformStructs.hpp"
#include <iostream>
#include "Texture.hpp"
#include <glm/gtc/type_ptr.hpp> 
#include <random>
SRender::SRender(BaseScene* scene) : mScene(scene)
{
}

void SRender::init()
{
	initImGui();
	initFramebuffers();
	generateSSAOKernel();
	generateSSAONoise();
	generateHaltonSequence();

}

void SRender::lateInit()
{
	calculateSceneBounds();

}

void SRender::render(float dt)
{
	buildRenderLists();
	LightData lightData;
	buildLightData(lightData);
	updateCameraUniforms();
	mPreviousJitter = mCurrentJitter;
	mCurrentJitter = mHaltonPattern[mJitterIndex] * mJitterScale;
	mJitterIndex = (mJitterIndex + 1) % HALTON_SAMPLES;
	GL_SCOPED_MARKER("Frame");
	GL_CHECK(glEnable(GL_DEPTH_TEST));
	GL_CHECK(glEnable(GL_CULL_FACE));
	GL_CHECK(glCullFace(GL_BACK));
	GL_CHECK(glFrontFace(GL_CCW));
	GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	GL_CHECK(glDepthFunc(GL_LESS));
	GL_CHECK(glDepthMask(GL_TRUE));


	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Execute the main render pipeline
	{
		GL_SCOPED_MARKER("Shadow Pass");
		GL_SCOPED_TIMER("Shadow Pass");
		shadowPass(lightData);
	}

	{
		GL_SCOPED_MARKER("Geometry Pass");
		GL_SCOPED_TIMER("G-Buffer");
		geometryPass();
	}
	{
		GL_SCOPED_MARKER("SSAO Pass");
		GL_SCOPED_TIMER("SSAO");
		ssaoPass();
	}

	{
		GL_SCOPED_MARKER("Lighting Pass");
		GL_SCOPED_TIMER("Deferred Lighting");
		mLightUBO->setData(&lightData, sizeof(LightData));
		lightingPass();
	}

	{
		GL_SCOPED_MARKER("Forward Pass");
		GL_SCOPED_TIMER("Forward Transparency");
		forwardPass();
	}

	{
		GL_SCOPED_MARKER("Post Process");
		GL_SCOPED_TIMER("Post Processing");
		postProcessPass(dt);
	}
	drawImGui();

}

void SRender::shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
void SRender::initImGui()
{
	GLFWwindow* window = GameManager::getGLFWWindow();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	float xscale, yscale;
	glfwGetWindowContentScale(window, &xscale, &yscale);
	float dpi_scale = (xscale + yscale) * 0.5f;

	io.FontGlobalScale = dpi_scale;
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(dpi_scale);

	int fbWidth, fbHeight;
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);


	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

void SRender::initFramebuffers()
{
	// Create uniform buffers
	mCameraUBO = std::make_shared<UniformBuffer>(sizeof(CameraData), CAMERA_BINDING);
	GL_LABEL_OBJECT(GL_BUFFER, mCameraUBO->getID(), "Camera UBO");
	mLightUBO = std::make_shared<UniformBuffer>(sizeof(LightData), LIGHT_BINDING);
	GL_LABEL_OBJECT(GL_BUFFER, mLightUBO->getID(), "Light UBO");


	// Directional & Spot shadow map FBO
	auto depthAttachment =
		std::vector<FrameBufferAttachmentSpecification>{
			{ FrameBufferAttachmentType::Depth, FrameBufferTextureFormat::Depth32F }
	};

	mDirectionalShadowMapBuffer = std::make_shared<FrameBuffer>(mShadowMapWidth, mShadowMapHeight, depthAttachment);
	GL_LABEL_OBJECT(GL_FRAMEBUFFER, mDirectionalShadowMapBuffer->getRendererID(), "Directional Shadow FBO");
	mDirectionalShadowMapBuffer->getDepthAttachment().setShadowSamplerParameters();
	if (!mDirectionalShadowMapBuffer->isComplete())
		throw std::runtime_error("Directional shadow map framebuffer setup failed!");

	mSpotShadowMapBuffer = std::make_shared<FrameBuffer>(mShadowMapWidth, mShadowMapHeight, depthAttachment);
	GL_LABEL_OBJECT(GL_FRAMEBUFFER, mSpotShadowMapBuffer->getRendererID(), "Spot Shadow FBO");
	mSpotShadowMapBuffer->getDepthAttachment().setShadowSamplerParameters();
	if (!mSpotShadowMapBuffer->isComplete())
		throw std::runtime_error("Spot shadow map framebuffer setup failed!");

	// Point shadow map FBO (cubemap)
	auto depthCubemapAttachment =
		std::vector<FrameBufferAttachmentSpecification>{
			{ FrameBufferAttachmentType::DepthCubemap, FrameBufferTextureFormat::Depth32F }
	};

	mPointShadwMapBuffer = std::make_shared<FrameBuffer>(mShadowMapWidth, mShadowMapHeight, depthCubemapAttachment);
	GL_LABEL_OBJECT(GL_FRAMEBUFFER, mPointShadwMapBuffer->getRendererID(), "Point Shadow FBO");
	mPointShadwMapBuffer->getDepthAttachment().setShadowSamplerParameters();
	if (!mPointShadwMapBuffer->isComplete())
		throw std::runtime_error("Point shadow map framebuffer setup failed!");

	// G-Buffer FBO
	std::vector<FrameBufferAttachmentSpecification> gBufferAttachments = {
		{ FrameBufferAttachmentType::Color,  FrameBufferTextureFormat::RGBA8 },
		{ FrameBufferAttachmentType::Color,  FrameBufferTextureFormat::RGBA8 },
		{ FrameBufferAttachmentType::Color,  FrameBufferTextureFormat::RGBA8 },
		{ FrameBufferAttachmentType::Color,  FrameBufferTextureFormat::RGBA8 },
		{ FrameBufferAttachmentType::Depth,  FrameBufferTextureFormat::Depth32F }
	};
	mGBuffer = std::make_shared<FrameBuffer>(settings::window_width, settings::window_height, gBufferAttachments);
	GL_LABEL_OBJECT(GL_FRAMEBUFFER, mGBuffer->getRendererID(), "G-Buffer FBO");
	if (!mGBuffer->isComplete())
	{
		throw std::runtime_error("GBuffer framebuffer setup failed!");
	}

	// HDR FBO
	std::vector<FrameBufferAttachmentSpecification> hdrAttachments = {
		{ FrameBufferAttachmentType::Color,  FrameBufferTextureFormat::RGBA16F },
		{ FrameBufferAttachmentType::Depth,  FrameBufferTextureFormat::Depth32F }
	};

	mHDRFrameBuffer = std::make_shared<FrameBuffer>(settings::window_width, settings::window_height, hdrAttachments);
	GL_LABEL_OBJECT(GL_FRAMEBUFFER, mHDRFrameBuffer->getRendererID(), "HDR FBO");
	if (!mHDRFrameBuffer->isComplete())
		throw std::runtime_error("HDR framebuffer setup failed!");

	// Ping-pong FBOs for bloom blur
	auto colorAttachment =
		std::vector<FrameBufferAttachmentSpecification>{
			{ FrameBufferAttachmentType::Color, FrameBufferTextureFormat::RGBA8 }
	};
	mBloomFrameBuffer = std::make_shared<FrameBuffer>(settings::window_width / 2, settings::window_height / 2, colorAttachment);
	for (auto& i : mPingPongFBO)
	{
		i = std::make_shared<FrameBuffer>(settings::window_width / 2, settings::window_height / 2, colorAttachment);
		if (!i->isComplete())
			throw std::runtime_error("Ping-pong framebuffer setup failed!");
	}
	mSSAOBuffer = std::make_shared<FrameBuffer>(
		settings::window_width / 2,
		settings::window_height / 2,
		colorAttachment
	);

	// SSAO Blur buffer
	mSSAOBlurBuffer = std::make_shared<FrameBuffer>(
		settings::window_width / 2,
		settings::window_height / 2,
		colorAttachment
	);

	mFXAAFrameBuffer = std::make_shared<FrameBuffer>(
		settings::window_width,
		settings::window_height,
		colorAttachment
	);
	mTAACurrentFrameBuffer = std::make_shared<FrameBuffer>(
		settings::window_width,
		settings::window_height,
		colorAttachment
	);
	mTAAPreviousFrameBuffer = std::make_shared<FrameBuffer>(
		settings::window_width,
		settings::window_height,
		colorAttachment
	);
	mSSRBuffer = std::make_shared<FrameBuffer>(
		settings::window_width,
		settings::window_height,
		colorAttachment
	);

}

void SRender::calculateSceneBounds()
{
	auto& registry = mScene->mEnttRegistry;
	auto view = registry.view<CModel, CTransform>();

	glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());
	bool foundAny = false;

	// Compute bounding box from all models in the scene
	for (auto entity : view)
	{
		const auto& transformComp = view.get<CTransform>(entity);
		const auto& modelComp = view.get<CModel>(entity);
		const auto& model = GameManager::mGraphicsManager->getModel(modelComp.name);
		auto& meshes = model->getMeshes();

		for (const auto& meshInstance : meshes)
		{
			auto& mesh = meshInstance.mesh;
			auto& localTransform = meshInstance.localTransform;
			auto& vertices = mesh->getVertices();

			glm::mat4 finalTransform = transformComp.modelMatrix * localTransform;
			for (const auto& vertex : vertices)
			{
				glm::vec4 worldPos = finalTransform * glm::vec4(vertex.position, 1.0f);
				glm::vec3 pos3 = glm::vec3(worldPos);
				min = glm::min(min, pos3);
				max = glm::max(max, pos3);
				foundAny = true;
			}
		}
	}

	if (!foundAny)
	{
		// Default to a small region if none found
		min = glm::vec3(-1.0f);
		max = glm::vec3(1.0f);
	}

	mSceneBounds.min = min;
	mSceneBounds.max = max;
	mSceneBounds.center = (min + max) * 0.5f;
	mSceneBounds.radius = glm::length(max - mSceneBounds.center);

	// Add a bit of padding
	float padding = mSceneBounds.radius * 0.1f;
	mSceneBounds.min -= glm::vec3(padding);
	mSceneBounds.max += glm::vec3(padding);
	mSceneBounds.radius *= 1.1f;
}


void SRender::buildLightData(LightData& lightData) const
{
	buildDirectionalLights(lightData);
	buildPointLights(lightData);
	buildSpotLights(lightData);
}

void SRender::buildDirectionalLights(LightData& lightData) const
{
	auto dirLightView = mScene->mEnttRegistry.view<CDirectionalLight>();
	unsigned numDirLights = 0;

	for (auto entity : dirLightView)
	{
		if (numDirLights >= MAX_DIRECTIONAL_LIGHTS) break;
		auto& light = dirLightView.get<CDirectionalLight>(entity);

		DirectionalLightData& data = lightData.directionalLights[numDirLights];
		data.direction = glm::vec4(light.direction, 0.0f);
		data.ambient = glm::vec4(light.ambient, 0.0f);
		data.diffuse = glm::vec4(light.diffuse, 0.0f);
		data.specular = glm::vec4(light.specular, 0.0f);

		// Compute light-space matrix
		glm::vec3 lightDir = glm::normalize(light.direction);
		glm::vec3 lightPos = mSceneBounds.center - lightDir * (mSceneBounds.radius * 2.0f);

		glm::vec3 up = (fabs(glm::dot(lightDir, glm::vec3(0, 1, 0))) > 0.99f)
			? glm::vec3(1, 0, 0)
			: glm::vec3(0, 1, 0);

		glm::mat4 lightView = glm::lookAt(lightPos, mSceneBounds.center, up);

		glm::vec3 boxCorners[8] = {
			glm::vec3(mSceneBounds.min.x, mSceneBounds.min.y, mSceneBounds.min.z),
			glm::vec3(mSceneBounds.max.x, mSceneBounds.min.y, mSceneBounds.min.z),
			glm::vec3(mSceneBounds.min.x, mSceneBounds.max.y, mSceneBounds.min.z),
			glm::vec3(mSceneBounds.max.x, mSceneBounds.max.y, mSceneBounds.min.z),
			glm::vec3(mSceneBounds.min.x, mSceneBounds.min.y, mSceneBounds.max.z),
			glm::vec3(mSceneBounds.max.x, mSceneBounds.min.y, mSceneBounds.max.z),
			glm::vec3(mSceneBounds.min.x, mSceneBounds.max.y, mSceneBounds.max.z),
			glm::vec3(mSceneBounds.max.x, mSceneBounds.max.y, mSceneBounds.max.z)
		};

		glm::vec3 lightSpaceMin(+FLT_MAX);
		glm::vec3 lightSpaceMax(-FLT_MAX);

		// Transform corners to light space
		for (int i = 0; i < 8; ++i)
		{
			glm::vec3 cornerLS = glm::vec3(lightView * glm::vec4(boxCorners[i], 1.0f));
			lightSpaceMin = glm::min(lightSpaceMin, cornerLS);
			lightSpaceMax = glm::max(lightSpaceMax, cornerLS);
		}

		float padding = glm::length(lightSpaceMax - lightSpaceMin) * 0.05f;
		lightSpaceMin -= glm::vec3(padding);
		lightSpaceMax += glm::vec3(padding);

		glm::mat4 lightProj = glm::ortho(
			lightSpaceMin.x, lightSpaceMax.x,
			lightSpaceMin.y, lightSpaceMax.y,
			-lightSpaceMax.z, -lightSpaceMin.z
		);

		data.lightSpaceMatrix = lightProj * lightView;
		numDirLights++;
	}

	lightData.counts.z = numDirLights; // .z holds # of directional lights
}

void SRender::buildPointLights(LightData& lightData) const
{
	auto pointLightView = mScene->mEnttRegistry.view<CPointLight>();
	unsigned numPointLights = 0;

	glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);

	for (auto entity : pointLightView)
	{
		if (numPointLights >= MAX_POINT_LIGHTS) break;
		auto& light = pointLightView.get<CPointLight>(entity);

		PointLightData& pld = lightData.pointLights[numPointLights];
		pld.position = glm::vec4(light.position, 0.0f);
		pld.ambient = glm::vec4(light.ambient, 0.0f);
		pld.diffuse = glm::vec4(light.diffuse, 0.0f);
		pld.specular = glm::vec4(light.specular, 0.0f);
		pld.attenuation = glm::vec4(light.constant, light.linear, light.quadratic, 0.0f);

		// Build shadow matrices for all 6 faces
		pld.shadowMatrices[0] = shadowProjection * glm::lookAt(light.position, light.position + glm::vec3(+1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f));
		pld.shadowMatrices[1] = shadowProjection * glm::lookAt(light.position, light.position + glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f));
		pld.shadowMatrices[2] = shadowProjection * glm::lookAt(light.position, light.position + glm::vec3(0.f, +1.f, 0.f), glm::vec3(0.f, 0.f, +1.f));
		pld.shadowMatrices[3] = shadowProjection * glm::lookAt(light.position, light.position + glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f));
		pld.shadowMatrices[4] = shadowProjection * glm::lookAt(light.position, light.position + glm::vec3(0.f, 0.f, +1.f), glm::vec3(0.f, -1.f, 0.f));
		pld.shadowMatrices[5] = shadowProjection * glm::lookAt(light.position, light.position + glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f));

		numPointLights++;
	}

	lightData.counts.x = numPointLights; // .x holds # of point lights
}

void SRender::buildSpotLights(LightData& lightData) const
{
	auto spotLightView = mScene->mEnttRegistry.view<CSpotLight>();
	unsigned numSpotLights = 0;

	for (auto entity : spotLightView)
	{
		if (numSpotLights >= MAX_SPOT_LIGHTS) break;
		auto& light = spotLightView.get<CSpotLight>(entity);

		SpotLightData& sld = lightData.spotLights[numSpotLights];
		sld.position = glm::vec4(light.position, 0.0f);
		sld.direction = glm::vec4(light.direction, 0.0f);
		sld.ambient = glm::vec4(light.ambient, 0.0f);
		sld.diffuse = glm::vec4(light.diffuse, 0.0f);
		sld.specular = glm::vec4(light.specular, 0.0f);
		sld.attenuation = glm::vec4(light.constant, light.linear, light.quadratic, 0.0f);
		sld.cutoffs = glm::vec4(light.innerCutoff, light.outerCutoff, 0.0f, 0.0f);

		// Build light-space matrix for spot lights
		glm::vec3 lightDir = glm::normalize(light.direction);
		glm::vec3 up = (fabs(glm::dot(lightDir, glm::vec3(0, 1, 0))) > 0.99f)
			? glm::vec3(1, 0, 0)
			: glm::vec3(0, 1, 0);

		glm::mat4 lightView = glm::lookAt(light.position, light.position + lightDir, up);

		glm::vec3 boxCorners[8] = {
			mSceneBounds.min,
			glm::vec3(mSceneBounds.max.x, mSceneBounds.min.y, mSceneBounds.min.z),
			glm::vec3(mSceneBounds.min.x, mSceneBounds.max.y, mSceneBounds.min.z),
			glm::vec3(mSceneBounds.max.x, mSceneBounds.max.y, mSceneBounds.min.z),
			glm::vec3(mSceneBounds.min.x, mSceneBounds.min.y, mSceneBounds.max.z),
			glm::vec3(mSceneBounds.max.x, mSceneBounds.min.y, mSceneBounds.max.z),
			glm::vec3(mSceneBounds.min.x, mSceneBounds.max.y, mSceneBounds.max.z),
			mSceneBounds.max
		};

		float minDepth = +FLT_MAX;
		float maxDepth = -FLT_MAX;

		for (auto& corner : boxCorners)
		{
			glm::vec4 cornerLS = lightView * glm::vec4(corner, 1.0f);
			float depth = -cornerLS.z; // negative because OpenGL looks down -Z
			minDepth = std::min(minDepth, depth);
			maxDepth = std::max(maxDepth, depth);
		}

		const float depthPadding = (maxDepth - minDepth) * 0.05f;
		const float nearPlane = std::max(0.1f, minDepth - depthPadding);
		const float farPlane = maxDepth + depthPadding;

		const float spotAngle = glm::acos(light.outerCutoff) * 2.0f; // outerCutoff is cos(halfAngle)
		glm::mat4 lightProj = glm::perspective(spotAngle, 1.0f, nearPlane, farPlane);

		sld.lightSpaceMatrix = lightProj * lightView;
		numSpotLights++;
	}

	lightData.counts.y = numSpotLights; // .y holds # of spot lights
}

void SRender::updateCameraUniforms() const
{

	CameraData cameraData;
	cameraData.cameraPos = glm::vec4(mScene->mCurrentCamera.mPosition, 0.0f);
	cameraData.view = mScene->mCurrentCamera.getViewMatrix();

	cameraData.projection = mScene->mCurrentCamera.getProjectionMatrix();
	if (mTAAEnabled)
	{
		cameraData.projection[2][0] += mCurrentJitter.x;
		cameraData.projection[2][1] += mCurrentJitter.y;
	}
	cameraData.viewProjection = mScene->mCurrentCamera.getViewProjectionMatrix();
	if (mTAAEnabled)
	{
		cameraData.viewProjection[2][0] += mCurrentJitter.x;
		cameraData.viewProjection[2][1] += mCurrentJitter.y;
	}
	// Set camera position for shaders

	cameraData.inverseView = mScene->mCurrentCamera.getInverseViewMatrix();
	cameraData.inverseProjection = mScene->mCurrentCamera.getInverseProjectionMatrix();
	cameraData.inverseViewProjection = mScene->mCurrentCamera.getInverseViewProjectionMatrix();

	// Previous matrices for reprojection
	cameraData.previousView = mScene->mCurrentCamera.getPreviousViewMatrix();
	cameraData.previousProjection = mScene->mCurrentCamera.getPreviousProjectionMatrix();
	cameraData.previousViewProjection = mScene->mCurrentCamera.getPreviousViewProjectionMatrix();

	// Upload data to the uniform buffer
	mCameraUBO->setData(&cameraData, sizeof(CameraData));

	// Store the current matrices for next frame
	mScene->mCurrentCamera.storePreviousMatrices();
}

void SRender::buildRenderLists()
{
	mOpaqueRenderList.clear();
	mTransparentRenderList.clear();
	mCulledMeshes = 0;

	glm::vec3 camPos = mScene->mCurrentCamera.mPosition;
	auto& registry = mScene->mEnttRegistry;
	auto modelView = registry.view<CModel, CTransform>();

	// Debug info for transparency
	int totalMeshes = 0;
	int transparentMeshes = 0;

	for (auto entity : modelView)
	{
		const auto& modelComp = modelView.get<CModel>(entity);
		const auto& transformComp = modelView.get<CTransform>(entity);
		auto model = GameManager::mGraphicsManager->getModel(modelComp.name);
		auto meshes = model->getMeshes();

		for (const auto& meshInstance : meshes)
		{
			totalMeshes++;
			// Get local bounding sphere
			glm::vec3 localCenter = meshInstance.mesh->getBoundingSphereCenter();
			float localRadius = meshInstance.mesh->getBoundingSphereRadius();

			// Calculate world transform
			glm::mat4 finalTransform = transformComp.modelMatrix * meshInstance.localTransform;

			// Extract scale without decomposing full matrix
			glm::vec3 scale(
				glm::length(glm::vec3(finalTransform[0])),
				glm::length(glm::vec3(finalTransform[1])),
				glm::length(glm::vec3(finalTransform[2]))
			);

			// Use maximum scale for radius
			float maxScale = std::max({ scale.x, scale.y, scale.z });
			float worldRadius = localRadius * maxScale;

			// Transform center directly
			glm::vec3 worldCenter = glm::vec3(finalTransform * glm::vec4(localCenter, 1.0f));

			// Perform frustum test
			if (!mScene->mCurrentCamera.isSphereInFrustum(worldCenter, worldRadius))
			{
				mCulledMeshes++;
				continue;
			}

			// Process visible meshes...
			float distanceToCamera = glm::length(worldCenter - camPos);
			auto material = meshInstance.mesh->getMaterial();
			bool isTransparent = false;

			// More robust transparency detection
			if (material)
			{
				// Check opacity value first
				float opacity = material->getOpacity();

				// Check if opacity texture exists - this always makes it transparent
				auto opacityTexture = material->getOpacityTexture();

				// Check if albedo texture has alpha channel
				auto albedoTexture = material->getAlbedoTexture();

				// Determine if transparent based on all criteria
				if (opacity < 0.999f || opacityTexture != nullptr)
				{
					isTransparent = true;
					transparentMeshes++;
				}
				// Check if material has albedo texture with alpha channel
				else if (albedoTexture && albedoTexture->hasAlpha())
				{
					isTransparent = true;
					transparentMeshes++;
				}
			}

			RenderItem item{ meshInstance.mesh, finalTransform, distanceToCamera };

			if (isTransparent)
				mTransparentRenderList.emplace_back(item);
			else
				mOpaqueRenderList.emplace_back(item);
		}
	}

	// Sort opaque objects by material to minimize state changes
	std::ranges::sort(mOpaqueRenderList,
	                  [](const RenderItem& a, const RenderItem& b)
	                  {
		                  auto ma = a.mesh->getMaterial();
		                  auto mb = b.mesh->getMaterial();
		                  return ma < mb;
	                  });

	// Sort transparent objects back-to-front
	std::ranges::sort(mTransparentRenderList,
	                  [](const RenderItem& a, const RenderItem& b)
	                  {
		                  return a.distance > b.distance;
	                  });

}





void SRender::shadowPass(const LightData& lightData) const
{
	GL_VALIDATE_STATE();
	GL_SCOPED_MARKER("Shadow Maps");
	GL_SCOPED_TIMER("Shadow Pass");
	// Front-face culling for shadow rendering
	GL_CHECK(glCullFace(GL_FRONT));
	GL_CHECK(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));

	// Render directional, spot, and point shadows
	{
		GL_SCOPED_MARKER("Directional Shadows");
		GL_SCOPED_TIMER("Directional Shadows");
		renderDirectionalShadows(lightData);
	}
	{
		GL_SCOPED_MARKER("Spot Shadows");
		GL_SCOPED_TIMER("Spot Shadows");
		renderSpotShadows(lightData);
	}
	{
		GL_SCOPED_MARKER("Point Shadows");
		GL_SCOPED_TIMER("Point Shadows");
		renderPointShadows(lightData);
	}


	// Restore back-face culling
	GL_CHECK(glCullFace(GL_BACK));
	GL_CHECK(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));


}

void SRender::geometryPass() const
{
	auto gBufferShader = GameManager::mGraphicsManager->getShader("GBuffer");
	gBufferShader->use();
	mGBuffer->setDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 });
	mGBuffer->bind();

	GL_CHECK(glEnable(GL_DEPTH_TEST));
	GL_CHECK(glDepthMask(GL_TRUE)); // enable depth writes for geometry pass
	GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT));
	GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
	//We dont want to clear depth pass
	GL_CHECK(glDisable(GL_BLEND)); // Disable blending for G-Buffer pass
	drawRenderList(mOpaqueRenderList, gBufferShader, true);
	
	mGBuffer->unbind();

}

void SRender::ssaoPass() const
{
	if (!ssaoEnabled) return;

	auto ssaoShader = GameManager::mGraphicsManager->getShader("SSAO");
	ssaoShader->use();

	// Set view and projection matrices

	// Send kernel and settings
	for (unsigned int i = 0; i < SSAO_KERNEL_SIZE; ++i)
		ssaoShader->setVec3("samples[" + std::to_string(i) + "]", mSSAOKernel[i]);

	ssaoShader->setFloat("radius", mSSAORadius);
	ssaoShader->setFloat("bias", mSSAOBias);
	ssaoShader->setFloat("power", mSSAOPower);
	ssaoShader->setInt("noiseSize", SSAO_NOISE_SIZE);
	ssaoShader->setInt("kernelSize", SSAO_KERNEL_SIZE);
	ssaoShader->setVec2("resolution", glm::vec2(settings::window_width, settings::window_height));
	ssaoShader->setFloat("minDistance", mSSAOMinDistance);
	ssaoShader->setFloat("maxDistance", mSSAOMaxDistance);

	// Bind G-Buffer textures using proper slots
	mGBuffer->getColorAttachment(1).bind(SSAOSlots::NORMAL_METALLIC);
	mGBuffer->getDepthAttachment().bind(SSAOSlots::DEPTH);
	mSSAONoise->bind(SSAOSlots::NOISE);

	ssaoShader->setInt("gDepth", SSAOSlots::DEPTH);
	ssaoShader->setInt("gNormalMetallic", SSAOSlots::NORMAL_METALLIC);
	ssaoShader->setInt("texNoise", SSAOSlots::NOISE);

	// Render SSAO texture
	mSSAOBuffer->bind();
	mSSAOBuffer->clear(GL_COLOR_BUFFER_BIT);
	gl::drawQuad();
	mSSAOBuffer->unbind();

	// Unbind textures after use
	mGBuffer->getColorAttachment(1).unbind(SSAOSlots::NORMAL_METALLIC);
	mGBuffer->getDepthAttachment().unbind(SSAOSlots::DEPTH);
	mSSAONoise->unbind(SSAOSlots::NOISE);

	// Blur SSAO texture
	auto blurShader = GameManager::mGraphicsManager->getShader("SSAOBlur");
	blurShader->use();

	mSSAOBlurBuffer->bind();
	mSSAOBlurBuffer->clear(GL_COLOR_BUFFER_BIT);

	// Bind textures for blur pass
	mSSAOBuffer->getColorAttachment(0).bind(SSAOSlots::SSAO_RAW_RESULT);
	mGBuffer->getColorAttachment(1).bind(SSAOSlots::NORMAL_METALLIC);
	mGBuffer->getDepthAttachment().bind(SSAOSlots::DEPTH);

	blurShader->setInt("ssaoInput", SSAOSlots::SSAO_RAW_RESULT);
	blurShader->setInt("gNormalMetallic", SSAOSlots::NORMAL_METALLIC);
	blurShader->setInt("gDepth", SSAOSlots::DEPTH);
	blurShader->setFloat("blurRadius", mSSAOBlurRadius);
	blurShader->setFloat("depthThreshold", mSSAOBlurDepthThreshold);
	blurShader->setFloat("normalThreshold", mSSAOBlurNormalThreshold);

	gl::drawQuad();
	mSSAOBlurBuffer->unbind();

	// Unbind textures after blur pass
	mSSAOBuffer->getColorAttachment(0).unbind(SSAOSlots::SSAO_RAW_RESULT);
	mGBuffer->getColorAttachment(1).unbind(SSAOSlots::NORMAL_METALLIC);
	mGBuffer->getDepthAttachment().unbind(SSAOSlots::DEPTH);
}



void SRender::lightingPass()
{
	int width, height;
	glfwGetFramebufferSize(GameManager::getGLFWWindow(), &width, &height);

	// 1. Copy depth buffer from G-Buffer to HDR framebuffer
	mGBuffer->blitTo(
		mHDRFrameBuffer,
		0, 0, width, height,
		0, 0, width, height,
		GL_DEPTH_BUFFER_BIT,
		GL_NEAREST
	);

	// 2. Bind HDR framebuffer and clear
	mHDRFrameBuffer->setDrawBuffers({ GL_COLOR_ATTACHMENT0 });
	mHDRFrameBuffer->bind();
	mHDRFrameBuffer->clear(GL_COLOR_BUFFER_BIT);

	GL_CHECK(glDisable(GL_DEPTH_TEST));
	GL_CHECK(glDisable(GL_CULL_FACE));

	// 3. Setup deferred shader
	auto deferredShader = GameManager::mGraphicsManager->getShader("Deferred");
	deferredShader->use();

	// 4. Bind G-Buffer textures
	mGBuffer->getColorAttachment(0).bind(GBufferSlots::ALBEDO_AO);
	mGBuffer->getColorAttachment(1).bind(GBufferSlots::NORMAL_METALLIC);
	mGBuffer->getColorAttachment(2).bind(GBufferSlots::ROUGH_EMISSIVE);
	mGBuffer->getDepthAttachment().bind(GBufferSlots::DEPTH);

	// 5. Bind shadow maps and environment maps
	bindSkyboxResources(deferredShader);

	// 6. Bind SSAO result if enabled
	if (ssaoEnabled)
	{
		mSSAOBlurBuffer->getColorAttachment(0).bind(SSAOSlots::SSAO_BLURRED_RESULT);
		deferredShader->setInt("ssaoTexture", SSAOSlots::SSAO_BLURRED_RESULT);
	}

	// 7. Set sampler uniforms
	deferredShader->setInt("gAlbedoAO", GBufferSlots::ALBEDO_AO);
	deferredShader->setInt("gNormalMetallic", GBufferSlots::NORMAL_METALLIC);
	deferredShader->setInt("gRoughEmissive", GBufferSlots::ROUGH_EMISSIVE);
	deferredShader->setInt("gDepth", GBufferSlots::DEPTH);
	deferredShader->setBool("ssaoEnabled", ssaoEnabled);

	// 8. Draw full-screen quad
	gl::drawQuad();

	// 9. Unbind G-Buffer textures
	mGBuffer->getColorAttachment(0).unbind(GBufferSlots::ALBEDO_AO);
	mGBuffer->getColorAttachment(1).unbind(GBufferSlots::NORMAL_METALLIC);
	mGBuffer->getColorAttachment(2).unbind(GBufferSlots::ROUGH_EMISSIVE);
	mGBuffer->getDepthAttachment().unbind(GBufferSlots::DEPTH);

	if (ssaoEnabled)
	{
		mSSAOBlurBuffer->getColorAttachment(0).unbind(SSAOSlots::SSAO_BLURRED_RESULT);
	}

	// 10. Unbind shadow maps and environment maps
	unbindSkyboxResources();

	// 11. Restore OpenGL state for skybox
	GL_CHECK(glEnable(GL_DEPTH_TEST));
	GL_CHECK(glEnable(GL_CULL_FACE));

	// 12. Draw skybox
	auto skyboxShader = GameManager::mGraphicsManager->getShader("Skybox");
	skyboxShader->use();

	const auto& viewMatrix = mScene->mCurrentCamera.getViewMatrix();
	glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(viewMatrix));

	skyboxShader->setMat4("view", viewNoTranslation);
	skyboxShader->setMat4("projection", mScene->mCurrentCamera.getProjectionMatrix());

	auto skybox = GameManager::mGraphicsManager->getEnvironmentMap("default");
	skybox->drawSkybox(skyboxShader);

	// 13. Unbind HDR framebuffer
	mHDRFrameBuffer->unbind();

	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
}




void SRender::postProcessPass(float dt)
{
	{
		GL_SCOPED_MARKER("SSR Process");
		GL_SCOPED_TIMER("SSR Process");
		ssrPass();
	}
	{
		GL_SCOPED_MARKER("TAA Process");
		GL_SCOPED_TIMER("TAA Process");
		taaPass();
	}

	//Extract bright lights after image is stabilized
	{
		GL_SCOPED_MARKER("Bloom Process");
		GL_SCOPED_TIMER("Bloom Process");
		bloomPass();
	}
	{
		GL_SCOPED_MARKER("HDR Process");
		GL_SCOPED_TIMER("HDR Process");
		hdrPass();
	}
	{
		GL_SCOPED_MARKER("FXAA Process");
		GL_SCOPED_TIMER("FXAA Process");
		fxaaPass();
	}


}

// ------------------------------------------------------
// Shadow Mapping
// ------------------------------------------------------

void SRender::renderDirectionalShadows(const LightData& lightData) const
{
	auto shadowMapShader = GameManager::mGraphicsManager->getShader("ShadowMap");

	if (lightData.counts.z > 0) // Use only the first directional light for now
	{
		shadowMapShader->use();
		shadowMapShader->setMat4("lightSpaceMatrix", lightData.directionalLights[0].lightSpaceMatrix);

		mDirectionalShadowMapBuffer->bind();
		mDirectionalShadowMapBuffer->clear(GL_DEPTH_BUFFER_BIT);

		drawRenderList(mOpaqueRenderList, shadowMapShader);

		mDirectionalShadowMapBuffer->unbind();
	}
}

void SRender::renderSpotShadows(const LightData& lightData) const
{
	auto shadowMapShader = GameManager::mGraphicsManager->getShader("ShadowMap");

	if (lightData.counts.y > 0) // Use only the first spot light for now
	{
		shadowMapShader->use();
		shadowMapShader->setMat4("lightSpaceMatrix", lightData.spotLights[0].lightSpaceMatrix);

		mSpotShadowMapBuffer->bind();
		mSpotShadowMapBuffer->clear(GL_DEPTH_BUFFER_BIT);
		drawRenderList(mOpaqueRenderList, shadowMapShader);
		mSpotShadowMapBuffer->unbind();
	}
}

void SRender::renderPointShadows(const LightData& lightData) const
{
	auto pointShadowMapShader = GameManager::mGraphicsManager->getShader("PointShadowMap");

	if (lightData.counts.x > 0) // Use only the first point light for now
	{
		pointShadowMapShader->use();
		pointShadowMapShader->setVec3("lightPos", lightData.pointLights[0].position);
		pointShadowMapShader->setFloat("far_plane", farPlane);

		for (int i = 0; i < 6; i++)
		{
			pointShadowMapShader->setMat4(
				"shadowMatrices[" + std::to_string(i) + "]",
				lightData.pointLights[0].shadowMatrices[i]
			);
		}

		mPointShadwMapBuffer->bind();
		mPointShadwMapBuffer->clear(GL_DEPTH_BUFFER_BIT);
		drawRenderList(mOpaqueRenderList, pointShadowMapShader);
		mPointShadwMapBuffer->unbind();
	}
}

// ------------------------------------------------------
// Post-Processing
// ------------------------------------------------------

void SRender::bloomPass() const
{
	if (!bloomEnabled)
		return;

	// 1. Extract bright parts of the scene with the physically-based approach
	auto bloomExtractShader = GameManager::mGraphicsManager->getShader("BloomExtract");
	bloomExtractShader->use();

	mBloomFrameBuffer->bind();
	mBloomFrameBuffer->clear(GL_COLOR_BUFFER_BIT);

	// Bind HDR buffer as input
	mHDRFrameBuffer->getColorAttachment(0).bind(PostProcessSlots::HDR_INPUT_OUTPUT);
	bloomExtractShader->setInt("hdrBuffer", PostProcessSlots::HDR_INPUT_OUTPUT);

	// Bind G-Buffer textures for physically-based extraction
	mGBuffer->getColorAttachment(1).bind(GBufferSlots::NORMAL_METALLIC);  // Normal + Metallic
	bloomExtractShader->setInt("gNormalMetallic", GBufferSlots::NORMAL_METALLIC);

	mGBuffer->getColorAttachment(2).bind(GBufferSlots::ROUGH_EMISSIVE);   // Roughness + Emissive
	bloomExtractShader->setInt("gRoughEmissive", GBufferSlots::ROUGH_EMISSIVE);

	// Set extraction parameters
	bloomExtractShader->setFloat("threshold", bloomThreshold);
	bloomExtractShader->setFloat("softThreshold", mBloomSoftThreshold);

	gl::drawQuad();

	// Unbind textures before moving to blur stage
	mHDRFrameBuffer->getColorAttachment(0).unbind(PostProcessSlots::HDR_INPUT_OUTPUT);
	mGBuffer->getColorAttachment(1).unbind(GBufferSlots::NORMAL_METALLIC);
	mGBuffer->getColorAttachment(2).unbind(GBufferSlots::ROUGH_EMISSIVE);
	mBloomFrameBuffer->unbind();

	// 2. Apply physically-based blur to the extracted bright areas
	auto blurShader = GameManager::mGraphicsManager->getShader("BloomBlur");
	blurShader->use();

	// Set common blur parameters
	blurShader->setFloat("scatteringCoefficient", mBloomScatteringCoefficient);
	blurShader->setInt("kernelSize", mBloomKernelSize);

	bool horizontal = true;
	bool firstIteration = true;

	// Perform multiple blur passes, alternating ping-pong FBO
	for (int i = 0; i < bloomBlurPasses; i++)
	{
		mPingPongFBO[horizontal]->bind();
		mPingPongFBO[horizontal]->clear(GL_COLOR_BUFFER_BIT);

		blurShader->setBool("horizontal", horizontal);

		if (firstIteration)
		{
			mBloomFrameBuffer->getColorAttachment(0).bind(PostProcessSlots::BLOOM_EXTRACT);
			blurShader->setInt("image", PostProcessSlots::BLOOM_EXTRACT);
			firstIteration = false;
		}
		else
		{
			mPingPongFBO[!horizontal]->getColorAttachment(0).bind(PostProcessSlots::BLOOM_EXTRACT);
			blurShader->setInt("image", PostProcessSlots::BLOOM_EXTRACT);
		}

		gl::drawQuad();

		// Unbind texture
		if (firstIteration)
			mBloomFrameBuffer->getColorAttachment(0).unbind(PostProcessSlots::BLOOM_EXTRACT);
		else
			mPingPongFBO[!horizontal]->getColorAttachment(0).unbind(PostProcessSlots::BLOOM_EXTRACT);

		horizontal = !horizontal;
	}

	mPingPongFBO[!horizontal]->unbind();
}



void SRender::hdrPass() const
{

	auto hdrShader = GameManager::mGraphicsManager->getShader("HDR");
	hdrShader->use();
	mHDRFrameBuffer->bind();


	hdrShader->setFloat("exposure", mExposure);
	hdrShader->setBool("hdr", mHDR);
	hdrShader->setBool("aces", mACES);

	hdrShader->setBool("bloom", bloomEnabled);
	hdrShader->setFloat("bloomStrength", bloomStrength);
	hdrShader->setFloat("brightness", mBrightness);
	hdrShader->setFloat("contrast", mContrast);
	hdrShader->setFloat("saturation", mSaturation);



	// HDR color
	mHDRFrameBuffer->getColorAttachment(0).bind(PostProcessSlots::HDR_INPUT_OUTPUT);
	hdrShader->setInt("hdrBuffer", PostProcessSlots::HDR_INPUT_OUTPUT);

	// Bloom color
	int finalPingPongIndex = (bloomBlurPasses % 2 == 0) ? 1 : 0;
	mPingPongFBO[finalPingPongIndex]->getColorAttachment(0).bind(PostProcessSlots::BLOOM_EXTRACT);
	hdrShader->setInt("bloomBuffer", PostProcessSlots::BLOOM_EXTRACT);

	gl::drawQuad();
	mHDRFrameBuffer->getColorAttachment(0).unbind(PostProcessSlots::HDR_INPUT_OUTPUT);
	mPingPongFBO[0]->getColorAttachment(0).unbind(PostProcessSlots::BLOOM_EXTRACT);
	mHDRFrameBuffer->unbind();

}

void SRender::taaPass()
{
	if (!mTAAEnabled) return;

	auto taaShader = GameManager::mGraphicsManager->getShader("TAA");
	taaShader->use();

	mTAACurrentFrameBuffer->bind();

	// Init history buffer on first frame
	if (mFirstFrame)
	{
		mTAACurrentFrameBuffer->clear(GL_COLOR_BUFFER_BIT);
		mTAAPreviousFrameBuffer->clear(GL_COLOR_BUFFER_BIT);
		mHDRFrameBuffer->getColorAttachment(0).bind(PostProcessSlots::HDR_INPUT_OUTPUT);
		taaShader->setInt("currentFrame", PostProcessSlots::HDR_INPUT_OUTPUT);
		taaShader->setInt("previousFrame", PostProcessSlots::HDR_INPUT_OUTPUT);
		mFirstFrame = false;
	}
	else
	{
		// Normal TAA pass bindings

		mHDRFrameBuffer->getColorAttachment(0).bind(PostProcessSlots::HDR_INPUT_OUTPUT);
		taaShader->setInt("currentFrame", PostProcessSlots::HDR_INPUT_OUTPUT);
		mTAAPreviousFrameBuffer->getColorAttachment(0).bind(PostProcessSlots::TAA_HISTORY);
		taaShader->setInt("previousFrame", PostProcessSlots::TAA_HISTORY);
	}

	taaShader->setFloat("blendFactor", mTAABlendFactor);
	taaShader->setVec2("resolution", glm::vec2(settings::window_width, settings::window_height));

	mGBuffer->getColorAttachment(3).bind(GBufferSlots::VELOCITY_REFLECTIVE);
	taaShader->setInt("velocityReflectiveMap", GBufferSlots::VELOCITY_REFLECTIVE);

	taaShader->setBool("showEdges", mShowEdges);
	taaShader->setFloat("edgeThreshold", mEdgeBlendThreshold);
	taaShader->setFloat("edgeResponsiveness", mEdgeResponsive);
	taaShader->setFloat("minBlend", mMinBlendAtEdges);


	gl::drawQuad();
	mHDRFrameBuffer->getColorAttachment(0).unbind(PostProcessSlots::HDR_INPUT_OUTPUT);
	mBloomFrameBuffer->unbind();
	mTAACurrentFrameBuffer->blitTo(mHDRFrameBuffer,
								   0, 0, settings::window_width, settings::window_height,
								   0, 0, settings::window_width, settings::window_height,
								   GL_COLOR_BUFFER_BIT,
								   GL_LINEAR);

	std::swap(mTAACurrentFrameBuffer, mTAAPreviousFrameBuffer);
	mTAAPreviousFrameBuffer->unbind();
	mGBuffer->getColorAttachment(3).unbind(GBufferSlots::VELOCITY_REFLECTIVE);

}






void SRender::fxaaPass()
{
	auto fxaaShader = GameManager::mGraphicsManager->getShader("FXAA");
	fxaaShader->use();


	// Use HDR framebuffer as input (which now contains the tonemapped result)
	mHDRFrameBuffer->getColorAttachment(0).bind(PostProcessSlots::HDR_INPUT_OUTPUT);
	fxaaShader->setInt("screenTexture", PostProcessSlots::HDR_INPUT_OUTPUT);
	fxaaShader->setBool("fxaaEnabled", mFXAAEnabled);

	// Set FXAA parameters
	fxaaShader->setVec2("inverseScreenSize",
						glm::vec2(1.0f / settings::window_width, 1.0f / settings::window_height));
	fxaaShader->setFloat("EDGE_THRESHOLD_MIN", mFXAAEdgeThreshholdMin);
	fxaaShader->setFloat("EDGE_THRESHOLD_MAX", mFXAAEdgeThreshholdMax);
	fxaaShader->setFloat("SUBPIXEL_QUALITY", mFXAASubPixelQuality);

	// Draw fullscreen quad to apply FXAA directly to the screen
	gl::drawQuad();
	mHDRFrameBuffer->getColorAttachment(0).unbind(PostProcessSlots::HDR_INPUT_OUTPUT);
}


void SRender::ssrPass() const
{
	if (!mSSREnabled) return;
	auto ssrShader = GameManager::mGraphicsManager->getShader("SSR");
	ssrShader->use();

	mGBuffer->getColorAttachment(1).bind(GBufferSlots::NORMAL_METALLIC);
	ssrShader->setInt("gNormalMetallic", GBufferSlots::NORMAL_METALLIC);

	mGBuffer->getColorAttachment(3).bind(GBufferSlots::VELOCITY_REFLECTIVE);
	ssrShader->setInt("gVelocityReflective", GBufferSlots::VELOCITY_REFLECTIVE);

	mGBuffer->getDepthAttachment().bind(GBufferSlots::DEPTH);
	ssrShader->setInt("gDepth", GBufferSlots::DEPTH);

	mHDRFrameBuffer->getColorAttachment(0).bind(PostProcessSlots::HDR_INPUT_OUTPUT);
	ssrShader->setInt("gSceneColor", PostProcessSlots::HDR_INPUT_OUTPUT);

	ssrShader->setFloat("uReflectionIntensity", mSSRReflectionIntensity);
	ssrShader->setFloat("uRayThickness", mSSRRayThickness);
	ssrShader->setInt("uMaxRaySteps", mSSRMaxRaySteps);
	ssrShader->setFloat("uMinReflectivity", mSSRMinReflectivity);
	ssrShader->setFloat("uReflectionFalloffDistance", mSSRReflectionFalloffDistance);
	ssrShader->setFloat("uRayOffset", mSSRRayOffset);


	mSSRBuffer->bind();
	mSSRBuffer->clear(GL_COLOR_BUFFER_BIT);
	gl::drawQuad();
	mSSRBuffer->blitTo(mHDRFrameBuffer,
					   0, 0, settings::window_width, settings::window_height,
					   0, 0, settings::window_width, settings::window_height,
					   GL_COLOR_BUFFER_BIT,
					   GL_LINEAR);
	mSSRBuffer->unbind();
	mGBuffer->getColorAttachment(1).unbind(GBufferSlots::NORMAL_METALLIC);
	mGBuffer->getColorAttachment(3).unbind(GBufferSlots::VELOCITY_REFLECTIVE);
	mGBuffer->getDepthAttachment().unbind(GBufferSlots::DEPTH);
	mHDRFrameBuffer->getColorAttachment(0).unbind(PostProcessSlots::HDR_INPUT_OUTPUT);



}









void SRender::bindSkyboxResources(const std::shared_ptr<Shader>& shader) const
{
	auto skybox = GameManager::mGraphicsManager->getEnvironmentMap("default");
	skybox->bindIrradiance(IBLSlots::IRRADIANCE);
	skybox->bindPrefilter(IBLSlots::PREFILTER);
	skybox->bindBRDFLUT(IBLSlots::BRDFLUT);

	shader->setInt("irradianceMap", IBLSlots::IRRADIANCE);
	shader->setInt("prefilterMap", IBLSlots::PREFILTER);
	shader->setInt("brdfLUT", IBLSlots::BRDFLUT);

	bindShadowMaps(shader);

	shader->setFloat("farPlane", farPlane);
	shader->setBool("enableShadows", mEnableShadows);
}
void SRender::unbindSkyboxResources() const
{
	// Unbind environment maps
	auto skybox = GameManager::mGraphicsManager->getEnvironmentMap("default");
	skybox->unbindIrradiance(IBLSlots::IRRADIANCE);
	skybox->unbindPrefilter(IBLSlots::PREFILTER);
	skybox->unbindBRDFLUT(IBLSlots::BRDFLUT);

	// Unbind shadow maps
	mDirectionalShadowMapBuffer->getDepthAttachment().unbind(ShadowSlots::DIRECTIONAL);
	mSpotShadowMapBuffer->getDepthAttachment().unbind(ShadowSlots::SPOT);
	mPointShadwMapBuffer->getDepthAttachment().unbind(ShadowSlots::POINT);
}

void SRender::bindShadowMaps(const std::shared_ptr<Shader>& shader) const
{
	mDirectionalShadowMapBuffer->getDepthAttachment().bind(ShadowSlots::DIRECTIONAL);
	shader->setInt("directionalShadowMap", ShadowSlots::DIRECTIONAL);

	mSpotShadowMapBuffer->getDepthAttachment().bind(ShadowSlots::SPOT);
	shader->setInt("spotShadowMap", ShadowSlots::SPOT);

	mPointShadwMapBuffer->getDepthAttachment().bind(ShadowSlots::POINT);
	shader->setInt("pointShadowMap", ShadowSlots::POINT);
}





void SRender::drawRenderList(const std::vector<RenderItem>& renderList, std::shared_ptr<Shader>& shader, bool bindMaterial)
{
	for (const auto& item : renderList)
	{
		glm::mat4 modelMatrix = item.transform;
		item.mesh->draw(shader, modelMatrix, bindMaterial);
	}
}

void SRender::drawImGui()
{
	// Main Rendering Settings Window
	ImGui::Begin("Rendering Settings");
	if (ImGui::CollapsingHeader("Camera"))
	{
		auto& camera = mScene->mCurrentCamera;
		ImGui::SliderFloat3("Position", glm::value_ptr(camera.mPosition), -25.0f, 25.0f);
		ImGui::SliderFloat("Yaw", &camera.mYaw, -180.0f, 180.0f);
		ImGui::SliderFloat("Pitch", &camera.mPitch, -89.0f, 89.0f);
		ImGui::SliderFloat("FOV", &camera.mFov, 1.0f, 120.0f);
		camera.updateCamera();
	}

	if (ImGui::CollapsingHeader("Post-Processing"))
	{
		// HDR Settings
		ImGui::Checkbox("Enable HDR", &mHDR);
		ImGui::SliderFloat("Exposure", &mExposure, 0.0f, 5.0f);
		ImGui::Checkbox("Enable ACES Tonemapping", &mACES);
		ImGui::SliderFloat("Brightness", &mBrightness, 0.0f, 2.0f);
		ImGui::SliderFloat("Contrast", &mContrast, 0.0f, 2.0f);
		ImGui::SliderFloat("Saturation", &mSaturation, 0.0f, 2.0f);


		// Bloom Settings
		ImGui::Separator();
		ImGui::Checkbox("Enable Bloom", &bloomEnabled);
		if (bloomEnabled)
		{
			ImGui::SliderFloat("Bloom Threshold", &bloomThreshold, 0.0f, 1.0f);
			ImGui::SliderFloat("Bloom Strength", &bloomStrength, 0.0f, 2.0f);
			ImGui::SliderInt("Blur Passes", &bloomBlurPasses, 1, 20);
			ImGui::SliderFloat("Soft Threshold", &mBloomSoftThreshold, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("Scattering Coefficient", &mBloomScatteringCoefficient, 0.1f, 10.0f, "%.1f");
			ImGui::SliderInt("Kernel Size", &mBloomKernelSize, 5, 31);
		}
		ImGui::Separator();
		ImGui::Checkbox("Enable SSR", &mSSREnabled);
		if (mSSREnabled)
		{
			ImGui::SliderFloat("Reflection Intensity", &mSSRReflectionIntensity, 0.0f, 5.0f);
			ImGui::SliderFloat("Ray Thickness", &mSSRRayThickness, 0.0f, 2.0f);
			ImGui::SliderInt("Max Ray Steps", &mSSRMaxRaySteps, 1, 1000);
			ImGui::SliderFloat("Min Reflectivity", &mSSRMinReflectivity, 0.0f, 1.0f);
			ImGui::SliderFloat("Reflection Falloff Distance", &mSSRReflectionFalloffDistance, 0.0f, 100.0f);
			ImGui::SliderFloat("Ray Offset", &mSSRRayOffset, 0.0f, 3.0f);
		}
		// SSAO Settings
		ImGui::Separator();
		ImGui::Checkbox("Enable SSAO", &ssaoEnabled);
		if (ssaoEnabled)
		{
			ImGui::SliderFloat("SSAO Radius", &mSSAORadius, 0.0f, 5.0f);
			ImGui::SliderFloat("SSAO Bias", &mSSAOBias, 0.0f, 0.5f);
			ImGui::SliderFloat("SSAO Power", &mSSAOPower, 0.0f, 5.0f);
			ImGui::SliderFloat("SSAO Min Distance", &mSSAOMinDistance, 0.0f, 0.5f);
			ImGui::SliderFloat("SSAO Max Distance", &mSSAOMaxDistance, 0.0f, 10.0f);
			ImGui::SliderFloat("SSAO Blur Radius", &mSSAOBlurRadius, 0.0f, 5.0f);
			ImGui::SliderFloat("SSAO Blur Depth Threshold", &mSSAOBlurDepthThreshold, 0.0f, 1.0f);
			ImGui::SliderFloat("SSAO Blur Normal Threshold", &mSSAOBlurNormalThreshold, 0.0f, 1.0f);
		}
		ImGui::Separator();
		ImGui::Checkbox("Enable FXAA", &mFXAAEnabled);
		if (mFXAAEnabled)
		{
			ImGui::SliderFloat("FXAA Edge Threshold Min", &mFXAAEdgeThreshholdMin, 0.0f, 0.2f);
			ImGui::SliderFloat("FXAA Edge Threshold Max", &mFXAAEdgeThreshholdMax, 0.0f, 0.5f);
			ImGui::SliderFloat("FXAA Subpixel Quality", &mFXAASubPixelQuality, 0.0f, 1.0f);
		}
		ImGui::Separator();
		ImGui::Separator();
		ImGui::Checkbox("Enable TAA", &mTAAEnabled);
		if (mTAAEnabled)
		{
			ImGui::Checkbox("Show Edges", &mShowEdges);
			ImGui::SliderFloat("TAA Blend Factor", &mTAABlendFactor, 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat("Jitter Scale", &mJitterScale, 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat("Blend Edge Threshold", &mEdgeBlendThreshold, 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat("Min Blend at Edges", &mMinBlendAtEdges, 0.0f, 1.0f, "%.3f");
			ImGui::SliderFloat("Edge Responsive", &mEdgeResponsive, 0.0f, 10.0f, "%.3f");
			ImGui::Text("Current Jitter: (%.3f, %.3f)", mCurrentJitter.x, mCurrentJitter.y);
			ImGui::Text("Previous Jitter: (%.3f, %.3f)", mPreviousJitter.x, mPreviousJitter.y);

			if (ImGui::TreeNode("Advanced TAA Settings"))
			{
				static bool showJitterPattern = false;
				ImGui::Checkbox("Show Jitter Pattern", &showJitterPattern);

				if (showJitterPattern)
				{
					ImGui::Text("Halton Pattern Samples:");
					ImGui::Indent();
					for (int i = 0; i < HALTON_SAMPLES; i++)
					{
						ImGui::Text("[%d]: (%.3f, %.3f)", i,
									mHaltonPattern[i].x, mHaltonPattern[i].y);
					}
					ImGui::Unindent();
				}

				// Pattern size info
				ImGui::Text("Pattern Size: %d samples", HALTON_SAMPLES);
				ImGui::Text("Current Sample: %d", mJitterIndex);

				if (ImGui::Button("Reset History"))
				{
					mFirstFrame = true; // Force history buffer reset
				}

				ImGui::TreePop();
			}
		}

	}

	if (ImGui::CollapsingHeader("Lighting"))
	{
		ImGui::Checkbox("Enable Shadows", &mEnableShadows);

		auto& registry = mScene->mEnttRegistry;

		// Directional Lights
		if (ImGui::TreeNode("Directional Lights"))
		{
			auto dirLightView = registry.view<CDirectionalLight>();
			int dirLightIndex = 0;
			for (auto entity : dirLightView)
			{
				auto& light = dirLightView.get<CDirectionalLight>(entity);
				ImGui::PushID(dirLightIndex);
				if (ImGui::TreeNode(("Light " + std::to_string(dirLightIndex)).c_str()))
				{
					ImGui::SliderFloat3("Direction", glm::value_ptr(light.direction), -1.0f, 1.0f);
					ImGui::ColorEdit3("Ambient", glm::value_ptr(light.ambient));
					ImGui::ColorEdit3("Diffuse", glm::value_ptr(light.diffuse));
					ImGui::ColorEdit3("Specular", glm::value_ptr(light.specular));
					ImGui::TreePop();
				}
				ImGui::PopID();
				dirLightIndex++;
			}
			ImGui::TreePop();
		}

		// Point Lights
		if (ImGui::TreeNode("Point Lights"))
		{
			auto pointLightView = registry.view<CPointLight>();
			int pointLightIndex = 0;
			for (auto entity : pointLightView)
			{
				auto& light = pointLightView.get<CPointLight>(entity);
				ImGui::PushID(pointLightIndex);
				if (ImGui::TreeNode(("Light " + std::to_string(pointLightIndex)).c_str()))
				{
					ImGui::SliderFloat3("Position", glm::value_ptr(light.position), -10.0f, 10.0f);
					ImGui::ColorEdit3("Ambient", glm::value_ptr(light.ambient));
					ImGui::ColorEdit3("Diffuse", glm::value_ptr(light.diffuse));
					ImGui::ColorEdit3("Specular", glm::value_ptr(light.specular));
					ImGui::SliderFloat("Constant", &light.constant, 0.0f, 1.0f);
					ImGui::SliderFloat("Linear", &light.linear, 0.0f, 1.0f);
					ImGui::SliderFloat("Quadratic", &light.quadratic, 0.0f, 1.0f);
					ImGui::TreePop();
				}
				ImGui::PopID();
				pointLightIndex++;
			}
			ImGui::TreePop();
		}

		// Spot Lights
		if (ImGui::TreeNode("Spot Lights"))
		{
			auto spotLightView = registry.view<CSpotLight>();
			int spotLightIndex = 0;
			for (auto entity : spotLightView)
			{
				auto& light = spotLightView.get<CSpotLight>(entity);
				ImGui::PushID(spotLightIndex);
				if (ImGui::TreeNode(("Light " + std::to_string(spotLightIndex)).c_str()))
				{
					ImGui::SliderFloat3("Position", glm::value_ptr(light.position), -10.0f, 10.0f);
					ImGui::SliderFloat3("Direction", glm::value_ptr(light.direction), -1.0f, 1.0f);
					ImGui::ColorEdit3("Ambient", glm::value_ptr(light.ambient));
					ImGui::ColorEdit3("Diffuse", glm::value_ptr(light.diffuse));
					ImGui::ColorEdit3("Specular", glm::value_ptr(light.specular));
					ImGui::SliderFloat("Inner Cutoff", &light.innerCutoff, 0.0f, glm::pi<float>());
					ImGui::SliderFloat("Outer Cutoff", &light.outerCutoff, 0.0f, glm::pi<float>());
					ImGui::TreePop();
				}
				ImGui::PopID();
				spotLightIndex++;
			}
			ImGui::TreePop();
		}
		if (ImGui::CollapsingHeader("Debug Settings"))
		{
			bool debugOutput = gl::isDebugOutputEnabled();
			if (ImGui::Checkbox("Debug Output", &debugOutput))
			{
				gl::enableDebugOutput(debugOutput);
			}

			bool profiling = gl::timer::isProfilingEnabled();
			if (ImGui::Checkbox("GPU Profiling", &profiling))
			{
				gl::timer::enableProfiling(profiling);
			}

			static bool breakOnError = true;
			if (ImGui::Checkbox("Break On Error", &breakOnError))
			{
				gl::setBreakOnError(breakOnError);
			}
		}
	}
	if (ImGui::CollapsingHeader("Scene Bounds"))
	{
		ImGui::SliderFloat3("Center", glm::value_ptr(mSceneBounds.center), -1000.0f, 1000.0f);
		ImGui::SliderFloat("Radius", &mSceneBounds.radius, 0.1f, 1000.0f);

		if (ImGui::TreeNode("Advanced"))
		{
			ImGui::SliderFloat3("Min", glm::value_ptr(mSceneBounds.min), -1000.0f, 1000.0f);
			ImGui::SliderFloat3("Max", glm::value_ptr(mSceneBounds.max), -1000.0f, 1000.0f);
			ImGui::TreePop();
		}

		if (ImGui::Button("Recalculate"))
			calculateSceneBounds();

		ImGui::SameLine();
		if (ImGui::Button("+10% Padding"))
		{
			float padding = mSceneBounds.radius * 0.1f;
			mSceneBounds.min -= glm::vec3(padding);
			mSceneBounds.max += glm::vec3(padding);
			mSceneBounds.radius *= 1.1f;
		}
	}


	ImGui::End();
	ImGui::Begin("Performance Statistics");
	{
		ImGuiIO& io = ImGui::GetIO();

		// FPS and frame timing
		ImGui::Text("FPS: %.1f (%.2f ms/frame)", io.Framerate, 1000.0f / io.Framerate);
		ImGui::Separator();

		// Culling Statistics
		ImGui::Text("Culling Statistics:");
		const int totalMeshes = mOpaqueRenderList.size() + mTransparentRenderList.size() + mCulledMeshes;
		ImGui::Text("Total Meshes: %d", totalMeshes);
		ImGui::Text("Visible Meshes: %d", mOpaqueRenderList.size() + mTransparentRenderList.size());
		ImGui::Text("Culled Meshes: %d", mCulledMeshes);
		float cullPercentage = (totalMeshes > 0) ? static_cast<float>(mCulledMeshes) / totalMeshes * 100.0f : 0.0f;
		ImGui::Text("Culling Percentage: %.1f%%", cullPercentage);

		// GPU Timings for each render pass
		if (gl::timer::isProfilingEnabled())
		{
			ImGui::Separator();
			ImGui::Text("GPU Timings (ms):");

			const float columnWidth = ImGui::GetWindowWidth() * 0.65f;
			ImGui::Columns(2, "TimingColumns", true);
			ImGui::SetColumnWidth(0, columnWidth);

			// Header
			ImGui::Text("Pass"); ImGui::NextColumn();
			ImGui::Text("Time (ms)"); ImGui::NextColumn();
			ImGui::Separator();

			// Common helper for displaying timing row
			auto displayTiming = [](const char* label, float time)
				{
					ImGui::Text("%s", label); ImGui::NextColumn();
					ImGui::Text("%.2f", time); ImGui::NextColumn();
				};

			// Main passes
			displayTiming("Light Data", gl::timer::getLastDuration("Light Data"));
			displayTiming("Shadow Pass", gl::timer::getLastDuration("Shadow Pass"));
			displayTiming("G-Buffer", gl::timer::getLastDuration("G-Buffer"));
			displayTiming("SSAO", gl::timer::getLastDuration("SSAO"));
			displayTiming("Deferred Lighting", gl::timer::getLastDuration("Deferred Lighting"));
			displayTiming("Forward Transparency", gl::timer::getLastDuration("Forward Transparency"));
			displayTiming("Post Processing", gl::timer::getLastDuration("Post Processing"));

			// Shadow sub-passes
			ImGui::Separator();
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Shadow Sub-passes:"); ImGui::NextColumn();
			ImGui::NextColumn();
			displayTiming("  Directional Shadows", gl::timer::getLastDuration("Directional Shadows"));
			displayTiming("  Spot Shadows", gl::timer::getLastDuration("Spot Shadows"));
			displayTiming("  Point Shadows", gl::timer::getLastDuration("Point Shadows"));

			// Post-processing sub-passes
			ImGui::Separator();
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Post-Processing Sub-passes:"); ImGui::NextColumn();
			ImGui::NextColumn();
			displayTiming("  SSR Process", gl::timer::getLastDuration("SSR Process"));
			displayTiming("  TAA Process", gl::timer::getLastDuration("TAA Process"));
			displayTiming("  Bloom Process", gl::timer::getLastDuration("Bloom Process"));
			displayTiming("  HDR Process", gl::timer::getLastDuration("HDR Process"));
			displayTiming("  FXAA Process", gl::timer::getLastDuration("FXAA Process"));

			// Memory statistics (if available)
			ImGui::Separator();
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Render Lists:"); ImGui::NextColumn();
			ImGui::NextColumn();
			displayTiming("  Opaque Objects", static_cast<float>(mOpaqueRenderList.size()));
			displayTiming("  Transparent Objects", static_cast<float>(mTransparentRenderList.size()));

			ImGui::Columns(1);
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.7f, 1.0f),
							   "Enable GPU Profiling in Debug Settings to see detailed timings");
		}
	}
	ImGui::End();


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void SRender::generateSSAOKernel()
{
	mSSAOKernel.resize(SSAO_KERNEL_SIZE);

	std::uniform_real_distribution<float> randomFloats(-1.0f, 1.0f);
	std::default_random_engine generator;

	for (unsigned int i = 0; i < SSAO_KERNEL_SIZE; ++i)
	{
		// Generate sample point in hemisphere
		glm::vec3 sample;
		do
		{
			sample = glm::vec3(
				randomFloats(generator) * 2.0f - 1.0f,
				randomFloats(generator) * 2.0f - 1.0f,
				randomFloats(generator)  // Only positive z for hemisphere
			);
		} while (glm::length(sample) > 1.0f);  // Reject points outside unit sphere

		sample = glm::normalize(sample);

		// Scale samples s.t. they're more aligned to center of kernel
		float scale = static_cast<float>(i) / SSAO_KERNEL_SIZE;
		scale = glm::lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;

		mSSAOKernel[i] = sample;
	}
}

void SRender::generateSSAONoise()
{
	// Create array of random rotation vectors in tangent space
	std::vector<glm::vec3> ssaoNoise;
	ssaoNoise.reserve(SSAO_NOISE_SIZE * SSAO_NOISE_SIZE);

	std::uniform_real_distribution<float> randomFloats(-1.0f, 1.0f);
	std::default_random_engine generator;

	// Generate random rotation vectors around z-axis (since we're working in tangent space)
	for (unsigned int i = 0; i < SSAO_NOISE_SIZE * SSAO_NOISE_SIZE; i++)
	{
		// Generate random rotation vectors in tangent space (only rotating around z-axis)
		glm::vec3 noise(
			randomFloats(generator) * 2.0f - 1.0f,  // Random between -1 and 1
			randomFloats(generator) * 2.0f - 1.0f,  // Random between -1 and 1
			0.0f                                     // We'll rotate around z-axis
		);
		ssaoNoise.push_back(noise);
	}

	// Create OpenGL texture for noise
	GLuint noiseTexture;
	GL_CHECK(glGenTextures(1, &noiseTexture));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, noiseTexture));
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SSAO_NOISE_SIZE, SSAO_NOISE_SIZE, 0, GL_RGB, GL_FLOAT, ssaoNoise.data()));

	// Set texture parameters - we want to repeat the noise texture
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

	// Create and store the texture object
	mSSAONoise = std::make_shared<Texture>(noiseTexture, SSAO_NOISE_SIZE, SSAO_NOISE_SIZE);
}

void SRender::generateHaltonSequence()
{
	mHaltonPattern.resize(HALTON_SAMPLES);

	auto halton = [](int index, int base) -> float
		{
			float f = 1.0f;
			float r = 0.0f;
			while (index > 0)
			{
				f /= base;
				r += f * (index % base);
				index /= base;
			}
			return r;
		};

	for (int i = 0; i < HALTON_SAMPLES; i++)
	{
		float x = 2.0f * halton(i + 1, 2) - 1.0f; // Base 2 for X
		float y = 2.0f * halton(i + 1, 3) - 1.0f;  // Base 3 for Y

		float u = x / settings::window_width;
		float v = y / settings::window_height;

		mHaltonPattern[i] = glm::vec2(u, v);
	}

}

void SRender::forwardPass()
{
	if (mTransparentRenderList.empty())
	{
		// Skip if no transparent objects
		return;
	}

	auto forwardShader = GameManager::mGraphicsManager->getShader("Forward");
	forwardShader->use();



	forwardShader->setBool("ssaoEnabled", ssaoEnabled); // Set once if SSAO state doesn't change per object

	bindSkyboxResources(forwardShader); // Binds IBL + Shadows, sets uniforms

	if (ssaoEnabled)
	{
		mSSAOBlurBuffer->getColorAttachment(0).bind(SSAOSlots::SSAO_BLURRED_RESULT);
		forwardShader->setInt("ssaoTexture", SSAOSlots::SSAO_BLURRED_RESULT); // Ensure uniform is set correctly
	}

	// --- Render to HDR buffer ---
	mHDRFrameBuffer->bind();

	GL_CHECK(glEnable(GL_BLEND));
	GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)); // Standard alpha blending
	GL_CHECK(glDepthMask(GL_FALSE)); // disable depth writes for transparent pass

	drawRenderList(mTransparentRenderList, forwardShader, true);

	GL_CHECK(glDisable(GL_BLEND));
	GL_CHECK(glDepthMask(GL_TRUE)); // restore depth writes

	mHDRFrameBuffer->unbind();

	if (ssaoEnabled)
	{
		// Unbind SSAO from its slot to be safe
		mSSAOBlurBuffer->getColorAttachment(0).unbind(SSAOSlots::SSAO_BLURRED_RESULT);
	}
	unbindSkyboxResources(); // Unbinds IBL + Shadows from their slots

}














