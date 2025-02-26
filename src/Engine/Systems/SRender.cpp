#include "pch.hpp"
#include "SRender.hpp"

#include "Engine/Components.hpp"
#include "Engine/Util/EngineUtil.hpp"
#include "Engine/Util/platform.hpp"

#include "edyn/edyn.hpp"
#include "Engine/Base/BaseScene.hpp"
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
}

void SRender::lateInit()
{
    calculateSceneBounds();
}

void SRender::render()
{
    GL_SCOPED_MARKER("Frame");
    OpenGlUtil::beginFrame(); // Reset GPU timers

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    buildRenderLists();

    LightData lightData;
    // Build and upload light data
    {
        GL_SCOPED_MARKER("Light Data Update");
        GL_SCOPED_TIMER("Light Data");
        buildLightData(lightData);
        updateCameraUniforms();
    }

    // Execute the main render pipeline
    {
        GL_SCOPED_MARKER("Shadow Pass");
        GL_SCOPED_TIMER("Shadow Pass");
        shadowPass(lightData);
    }

    {
        GL_SCOPED_MARKER("Depth Pass");
        GL_SCOPED_TIMER("Depth Pass");
        depthPass();
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
        GL_SCOPED_MARKER("Post Process");
        GL_SCOPED_TIMER("Post Processing");
        postProcessPass();
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
    GLFWwindow* window = GameManager::get_glfw_window();
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
    mCameraUBO = UniformBuffer::createUniformBuffer(sizeof(CameraData), CAMERA_BINDING);
    GL_LABEL_OBJECT(GL_BUFFER, mCameraUBO->getID(), "Camera UBO");
    mLightUBO = UniformBuffer::createUniformBuffer(sizeof(LightData), LIGHT_BINDING);
    GL_LABEL_OBJECT(GL_BUFFER, mLightUBO->getID(), "Light UBO");

    // Directional & Spot shadow map FBO
    auto depthAttachment =
        std::vector<FrameBufferAttachmentSpecification>{
            { FrameBufferAttachmentType::Depth, FrameBufferTextureFormat::Depth32F }
    };

    mDirectionalShadowMapBuffer = FrameBuffer::createFrameBuffer(mShadowMapWidth, mShadowMapHeight, depthAttachment);
    GL_LABEL_OBJECT(GL_FRAMEBUFFER, mDirectionalShadowMapBuffer->getRendererID(), "Directional Shadow FBO");
    mDirectionalShadowMapBuffer->getDepthAttachment()->setShadowSamplerParameters();
    if (!mDirectionalShadowMapBuffer->isComplete() || !mDirectionalShadowMapBuffer->getDepthAttachment())
        throw std::runtime_error("Directional shadow map framebuffer setup failed!");

    mSpotShadowMapBuffer = FrameBuffer::createFrameBuffer(mShadowMapWidth, mShadowMapHeight, depthAttachment);
    GL_LABEL_OBJECT(GL_FRAMEBUFFER, mSpotShadowMapBuffer->getRendererID(), "Spot Shadow FBO");
    mSpotShadowMapBuffer->getDepthAttachment()->setShadowSamplerParameters();
    if (!mSpotShadowMapBuffer->isComplete() || !mSpotShadowMapBuffer->getDepthAttachment())
        throw std::runtime_error("Spot shadow map framebuffer setup failed!");

    // Point shadow map FBO (cubemap)
    auto depthCubemapAttachment =
        std::vector<FrameBufferAttachmentSpecification>{
            { FrameBufferAttachmentType::DepthCubemap, FrameBufferTextureFormat::Depth32F }
    };

    mPointShadwMapBuffer = FrameBuffer::createFrameBuffer(mShadowMapWidth, mShadowMapHeight, depthCubemapAttachment);
    GL_LABEL_OBJECT(GL_FRAMEBUFFER, mPointShadwMapBuffer->getRendererID(), "Point Shadow FBO");
    mPointShadwMapBuffer->getDepthAttachment()->setShadowSamplerParameters();
    if (!mPointShadwMapBuffer->isComplete())
        throw std::runtime_error("Point shadow map framebuffer setup failed!");

	// G-Buffer FBO
	std::vector<FrameBufferAttachmentSpecification> gBufferAttachments = {
		{ FrameBufferAttachmentType::Color,  FrameBufferTextureFormat::RGBA16F },
		{ FrameBufferAttachmentType::Color,  FrameBufferTextureFormat::RGBA16F },
		{ FrameBufferAttachmentType::Color,  FrameBufferTextureFormat::RGBA16F },
		{ FrameBufferAttachmentType::Depth,  FrameBufferTextureFormat::Depth32F }
	};
	mGBuffer = FrameBuffer::createFrameBuffer(settings::window_width, settings::window_height, gBufferAttachments);
    GL_LABEL_OBJECT(GL_FRAMEBUFFER, mGBuffer->getRendererID(), "G-Buffer FBO");
    if (!mGBuffer->isComplete())
    {
		throw std::runtime_error("GBuffer framebuffer setup failed!");
    }

    // HDR FBO
    std::vector<FrameBufferAttachmentSpecification> hdrAttachments = {
        { FrameBufferAttachmentType::Color,  FrameBufferTextureFormat::RGBA16F },
        { FrameBufferAttachmentType::Color,  FrameBufferTextureFormat::RGBA16F },
        { FrameBufferAttachmentType::Depth,  FrameBufferTextureFormat::Depth32F }
    };

    mHDRFrameBuffer = FrameBuffer::createFrameBuffer(settings::window_width, settings::window_height, hdrAttachments);
    GL_LABEL_OBJECT(GL_FRAMEBUFFER, mHDRFrameBuffer->getRendererID(), "HDR FBO");
    if (!mHDRFrameBuffer->isComplete())
        throw std::runtime_error("HDR framebuffer setup failed!");

    // Ping-pong FBOs for bloom blur
    auto colorAttachment =
        std::vector<FrameBufferAttachmentSpecification>{
            { FrameBufferAttachmentType::Color, FrameBufferTextureFormat::RGBA16F }
    };

    for (int i = 0; i < 2; i++)
    {
        mPingPongFBO[i] = FrameBuffer::createFrameBuffer(settings::window_width, settings::window_height, colorAttachment);
        if (!mPingPongFBO[i]->isComplete())
            throw std::runtime_error("Ping-pong framebuffer setup failed!");
    }
    std::vector<FrameBufferAttachmentSpecification> ssaoAttachments = {
        { FrameBufferAttachmentType::Color, FrameBufferTextureFormat::R16F }
    };
    mSSAOBuffer = FrameBuffer::createFrameBuffer(
        settings::window_width, 
        settings::window_height, 
        ssaoAttachments
    );

    // SSAO Blur buffer
    mSSAOBlurBuffer = FrameBuffer::createFrameBuffer(
        settings::window_width, 
        settings::window_height, 
        ssaoAttachments
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
        auto& model = GameManager::mGraphicsManager->getModel(modelComp.name);
        auto& meshes = model->getMeshes();

        for (const auto& meshInstance : meshes)
        {
            auto& mesh = meshInstance.mesh;
            auto& localTransform = meshInstance.localTransform;
            auto& vertices = mesh->getVertices();

            glm::mat4 finalTransform = transformComp.model_matrix * localTransform;
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

// ------------------------------------------------------
// Light Data Building
// ------------------------------------------------------

void SRender::buildLightData(LightData& lightData)
{
    buildDirectionalLights(lightData);
    buildPointLights(lightData);
    buildSpotLights(lightData);
}

void SRender::buildDirectionalLights(LightData& lightData)
{
    auto dirLightView = mScene->mEnttRegistry.view<CDirectionalLight>();
    int numDirLights = 0;

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

void SRender::buildPointLights(LightData& lightData)
{
    auto pointLightView = mScene->mEnttRegistry.view<CPointLight>();
    int numPointLights = 0;

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

void SRender::buildSpotLights(LightData& lightData)
{
    auto spotLightView = mScene->mEnttRegistry.view<CSpotLight>();
    int numSpotLights = 0;

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

        float depthPadding = (maxDepth - minDepth) * 0.05f;
        float nearPlane = std::max(0.1f, minDepth - depthPadding);
        float farPlane = maxDepth + depthPadding;

        float spotAngle = glm::acos(light.outerCutoff) * 2.0f; // outerCutoff is cos(halfAngle)
        glm::mat4 lightProj = glm::perspective(spotAngle, 1.0f, nearPlane, farPlane);

        sld.lightSpaceMatrix = lightProj * lightView;
        numSpotLights++;
    }

    lightData.counts.y = numSpotLights; // .y holds # of spot lights
}

// ------------------------------------------------------
// Render Passes
// ------------------------------------------------------

void SRender::updateCameraUniforms()
{
    const auto& viewMatrix = mScene->mCurrentCamera.GetViewMatrix();
    const auto& projMatrix = mScene->mCurrentCamera.GetProjectionMatrix();

    CameraData cameraData;
    cameraData.view = viewMatrix;
    cameraData.projection = projMatrix;
    cameraData.cameraPos = glm::vec4(mScene->mCurrentCamera.Position, 0.0f);

    mCameraUBO->setData(&cameraData, sizeof(CameraData));
}

//void SRender::buildRenderLists()
//{
//	//Collect models to render
//    //Collect transforms
//    //collect and organize meshes and materials and textures
//	//Find transparancy and sort
//    //Frustum culling
//    auto& registry = mScene->mEnttRegistry;
//    auto modelView = registry.view<CModel, CTransform>();
//
//}
void SRender::buildRenderLists()
{
    mOpaqueRenderList.clear();
    mTransparentRenderList.clear();
    mCulledMeshes = 0;

    glm::vec3 camPos = mScene->mCurrentCamera.Position;
    auto& registry = mScene->mEnttRegistry;
    auto modelView = registry.view<CModel, CTransform>();


    for (auto entity : modelView)
    {
        const auto& modelComp = modelView.get<CModel>(entity);
        const auto& transformComp = modelView.get<CTransform>(entity);
        auto model = GameManager::mGraphicsManager->getModel(modelComp.name);
        auto meshes = model->getMeshes();

        for (const auto& meshInstance : meshes)
        {
            // Get local bounding sphere
            glm::vec3 localCenter = meshInstance.mesh->getBoundingSphereCenter();
            float localRadius = meshInstance.mesh->getBoundingSphereRadius();

            // Calculate world transform
            glm::mat4 finalTransform = transformComp.model_matrix * meshInstance.localTransform;


            // Extract scale without decomposing full matrix
            glm::vec3 scale(
                glm::length(glm::vec3(finalTransform[0])),
                glm::length(glm::vec3(finalTransform[1])),
                glm::length(glm::vec3(finalTransform[2]))
            );

            // Use maximum scale for radius
            float maxScale = std::max({scale.x, scale.y, scale.z});
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
            float opacity = material ? material->getOpacity() : 1.0f;

            if (opacity < 1.0f || (material && material->getOpacityTexture() != nullptr))
                isTransparent = true;

            RenderItem item{meshInstance.mesh, finalTransform, distanceToCamera};

            if (isTransparent)
                mTransparentRenderList.emplace_back(item);
            else
                mOpaqueRenderList.emplace_back(item);
        }
    }

    // Sort transparent objects back-to-front
    std::sort(mTransparentRenderList.begin(), mTransparentRenderList.end(),
              [](const RenderItem& a, const RenderItem& b) {
                  return a.distance > b.distance;
              });
}




void SRender::shadowPass(const LightData& lightData)
{
    GL_VALIDATE_STATE();
    GL_SCOPED_MARKER("Shadow Maps");
    GL_SCOPED_TIMER("Shadow Pass");
    // Front-face culling for shadow rendering
    GL_CHECK(glCullFace(GL_FRONT));
    
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
    

}

void SRender::depthPass()
{
    auto depthShader = GameManager::mGraphicsManager->getShader("Depth");
    depthShader->use();
    // Only write to depth buffer
	mGBuffer->setViewport(0, 0, settings::window_width, settings::window_height);
	mGBuffer->bind();
    GL_CHECK(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
    // Clear depth
    GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT));
    GL_CHECK(glDepthMask(GL_TRUE));
    // Draw all opaque geometry
	drawRenderList(mOpaqueRenderList, depthShader);
	GL_CHECK(glDepthMask(GL_FALSE));
    // Re-enable color writes for subsequent passes
    GL_CHECK(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
    mGBuffer->unbind();
}

void SRender::geometryPass()
{
	auto gBufferShader = GameManager::mGraphicsManager->getShader("GBuffer");
	gBufferShader->use();
	mGBuffer->setViewport(0, 0, settings::window_width, settings::window_height);
	mGBuffer->setDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 });
	mGBuffer->bind();
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glDepthFunc(GL_LEQUAL));  // Use LEQUAL to render fragments at same depth
	GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
    //We dont want to clear depth pass
    GL_CHECK(glDisable(GL_BLEND)); // Disable blending for G-Buffer pass
	drawRenderList(mOpaqueRenderList, gBufferShader, true);

    //GL_CHECK(glDepthMask(GL_TRUE));
    GL_CHECK(glDepthFunc(GL_LESS));
	mGBuffer->unbind();

}

void SRender::ssaoPass()
{
    if (!ssaoEnabled) return;

    auto ssaoShader = GameManager::mGraphicsManager->getShader("SSAO");
    ssaoShader->use();

    // Set view and projection matrices
    ssaoShader->setMat4("projection", mScene->mCurrentCamera.GetProjectionMatrix());
    ssaoShader->setMat4("view", mScene->mCurrentCamera.GetViewMatrix());
    ssaoShader->setMat4("inverseProjection", mScene->mCurrentCamera.GetInverseProjectionMatrix());

    // Send kernel and settings
    for (unsigned int i = 0; i < SSAO_KERNEL_SIZE; ++i)
        ssaoShader->setVec3("samples[" + std::to_string(i) + "]", mSSAOKernel[i]);

    ssaoShader->setFloat("radius", mSSAORadius);
    ssaoShader->setFloat("bias", mSSAOBias);
    ssaoShader->setFloat("power", mSSAOPower);
    ssaoShader->setInt("noiseSize", SSAO_NOISE_SIZE);
    ssaoShader->setInt("kernelSize", SSAO_KERNEL_SIZE);
	ssaoShader->setVec2("resolution", glm::vec2(settings::window_width, settings::window_height));

    // Bind G-Buffer textures using proper slots
    mGBuffer->getColorAttachment(1)->bind(SSAOSlots::NORMAL_METALLIC);
    mGBuffer->getDepthAttachment()->bind(SSAOSlots::DEPTH);
    mSSAONoise->bind(SSAOSlots::NOISE);

    ssaoShader->setInt("gDepth", SSAOSlots::DEPTH);
    ssaoShader->setInt("gNormalMetallic", SSAOSlots::NORMAL_METALLIC);
    ssaoShader->setInt("texNoise", SSAOSlots::NOISE);

    // Render SSAO texture
    mSSAOBuffer->bind();
    mSSAOBuffer->setViewport(0, 0, settings::window_width, settings::window_height);
    OpenGlUtil::drawQuad();
    mSSAOBuffer->unbind();

    // Blur SSAO texture
    auto blurShader = GameManager::mGraphicsManager->getShader("SSAOBlur");
    blurShader->use();

    mSSAOBlurBuffer->bind();
    mSSAOBlurBuffer->setViewport(0, 0, settings::window_width, settings::window_height);
    mSSAOBuffer->getColorAttachment(0)->bind(SSAOSlots::SSAO);
    blurShader->setInt("ssaoInput", SSAOSlots::SSAO);

    mGBuffer->getColorAttachment(1)->bind(SSAOSlots::NORMAL_METALLIC);
	blurShader->setInt("gNormalMetallic", SSAOSlots::NORMAL_METALLIC);
    mGBuffer->getDepthAttachment()->bind(SSAOSlots::DEPTH);
	blurShader->setInt("gDepth", SSAOSlots::DEPTH);

	blurShader->setFloat("blurRadius", mSSAOBlurRadius);
	blurShader->setFloat("depthThereshold", mSSAOBlurDepthThreshold);
    blurShader->setFloat("normalThreshold", mSSAOBlurNormalThreshold);

    OpenGlUtil::drawQuad();
    mSSAOBlurBuffer->unbind();
}



void SRender::lightingPass()
{
    GL_VALIDATE_STATE();
    GL_SCOPED_MARKER("Lighting");
    GL_SCOPED_TIMER("Deferred Lighting");
    int width, height;
    glfwGetFramebufferSize(GameManager::get_glfw_window(), &width, &height);
    GL_CHECK(glViewport(0, 0, width, height));

    // 1. Setup OpenGL state for deferred lighting pass
    mGBuffer->bindRead();
    mHDRFrameBuffer->bindDraw();
    GL_CHECK(glBlitFramebuffer(
        0, 0, width, height,
        0, 0, width, height,
        GL_DEPTH_BUFFER_BIT,
        GL_NEAREST
    ));

    // 2. Bind HDR framebuffer and clear
    mHDRFrameBuffer->setViewport(0, 0, width, height);
    mHDRFrameBuffer->setDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });
    mHDRFrameBuffer->bind();

    
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
    GL_CHECK(glDisable(GL_DEPTH_TEST));      // We don't need depth testing for full-screen quad
    GL_CHECK(glDisable(GL_CULL_FACE));       // We want to render both sides of the quad
    GL_CHECK(glDisable(GL_BLEND));           // No blending needed for initial lighting pass
    // 3. Setup deferred shader and bind resources
    auto deferredShader = GameManager::mGraphicsManager->getShader("Deferred");
    deferredShader->use();

    bindSkyboxResources(deferredShader);

    // 4. Bind G-Buffer textures
    mGBuffer->getColorAttachment(0)->bind(GBufferSlots::ALBEDO_AO);
    mGBuffer->getColorAttachment(1)->bind(GBufferSlots::NORMAL_METALLIC);
    mGBuffer->getColorAttachment(2)->bind(GBufferSlots::ROUGH_EMISSIVE);
    mGBuffer->getDepthAttachment()->bind(GBufferSlots::DEPTH);

    // 5. Set sampler uniforms
    deferredShader->setInt("gAlbedoAO", GBufferSlots::ALBEDO_AO);
    deferredShader->setInt("gNormalMetallic", GBufferSlots::NORMAL_METALLIC);
    deferredShader->setInt("gRoughEmissive", GBufferSlots::ROUGH_EMISSIVE);
    deferredShader->setInt("gDepth", GBufferSlots::DEPTH);
    mSSAOBlurBuffer->getColorAttachment(0)->bind(SSAOSlots::SSAO_BLUR);
    deferredShader->setInt("ssaoTexture", SSAOSlots::SSAO_BLUR);
    deferredShader->setBool("ssaoEnabled", ssaoEnabled);
    // 6. Draw full-screen quad
    OpenGlUtil::drawQuad();

    // 7. Restore OpenGL state for skybox
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glEnable(GL_CULL_FACE));

    //// 8. Draw skybox...
    auto skyboxShader = GameManager::mGraphicsManager->getShader("Skybox");
    skyboxShader->use();

    const auto& viewMatrix = mScene->mCurrentCamera.GetViewMatrix();
    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(viewMatrix));

    skyboxShader->setMat4("view", viewNoTranslation);
    skyboxShader->setMat4("projection", mScene->mCurrentCamera.GetProjectionMatrix());

    auto skybox = GameManager::mGraphicsManager->getEnvironmentMap("default");
    skybox->drawSkybox(skyboxShader);


    mHDRFrameBuffer->unbind();
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

}




void SRender::postProcessPass()
{
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
}

// ------------------------------------------------------
// Shadow Mapping
// ------------------------------------------------------

void SRender::renderDirectionalShadows(const LightData& lightData)
{
    auto shadowMapShader = GameManager::mGraphicsManager->getShader("ShadowMap");

    if (lightData.counts.z > 0) // Use only the first directional light for now
    {
        shadowMapShader->use();
        shadowMapShader->setMat4("lightSpaceMatrix", lightData.directionalLights[0].lightSpaceMatrix);

        mDirectionalShadowMapBuffer->setViewport(0, 0, mShadowMapWidth, mShadowMapHeight);
        mDirectionalShadowMapBuffer->bind();
        mDirectionalShadowMapBuffer->clear(GL_DEPTH_BUFFER_BIT);
        GL_CHECK(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
		drawRenderList(mOpaqueRenderList, shadowMapShader);
        GL_CHECK(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
        mDirectionalShadowMapBuffer->unbind();
    }
}

void SRender::renderSpotShadows(const LightData& lightData)
{
    auto shadowMapShader = GameManager::mGraphicsManager->getShader("ShadowMap");

    if (lightData.counts.y > 0) // Use only the first spot light for now
    {
        shadowMapShader->use();
        shadowMapShader->setMat4("lightSpaceMatrix", lightData.spotLights[0].lightSpaceMatrix);

        mSpotShadowMapBuffer->setViewport(0, 0, mShadowMapWidth, mShadowMapHeight);
        mSpotShadowMapBuffer->bind();
        mSpotShadowMapBuffer->clear(GL_DEPTH_BUFFER_BIT);
        GL_CHECK(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
		drawRenderList(mOpaqueRenderList, shadowMapShader);
        GL_CHECK(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
        mSpotShadowMapBuffer->unbind();
    }
}

void SRender::renderPointShadows(const LightData& lightData)
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

        mPointShadwMapBuffer->setViewport(0, 0, mShadowMapWidth, mShadowMapHeight);
        mPointShadwMapBuffer->bind();
        mPointShadwMapBuffer->clear(GL_DEPTH_BUFFER_BIT);
        GL_CHECK(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
		drawRenderList(mOpaqueRenderList, pointShadowMapShader);
        GL_CHECK(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
        mPointShadwMapBuffer->unbind();
    }
}

// ------------------------------------------------------
// Post-Processing
// ------------------------------------------------------

void SRender::bloomPass()
{

    auto blurShader = GameManager::mGraphicsManager->getShader("Blur");
    blurShader->use();

    bool horizontal = true;
    bool firstIteration = true;

    // Perform multiple Gaussian blur passes, alternating ping-pong FBO
    for (int i = 0; i < blurPasses; i++)
    {
        mPingPongFBO[horizontal]->bind();
        mPingPongFBO[horizontal]->setViewport(0, 0, settings::window_width, settings::window_height);
        blurShader->setBool("horizontal", horizontal);

        if (firstIteration)
        {
            mHDRFrameBuffer->getColorAttachment(1)->bind(PostProcessSlots::BLOOM);
            blurShader->setInt("image", PostProcessSlots::BLOOM);
            firstIteration = false;
        }
        else
        {
            mPingPongFBO[!horizontal]->getColorAttachment(0)->bind(PostProcessSlots::BLOOM);
            blurShader->setInt("image", PostProcessSlots::BLOOM);
        }

        OpenGlUtil::drawQuad();
        horizontal = !horizontal;
    }

    mPingPongFBO[!horizontal]->unbind();
}

void SRender::hdrPass()
{
    int width, height;
    glfwGetFramebufferSize(GameManager::get_glfw_window(), &width, &height);
    GL_CHECK(glViewport(0, 0, width, height));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    auto hdrShader = GameManager::mGraphicsManager->getShader("HDR");
    hdrShader->use();

    hdrShader->setFloat("exposure", mExposure);
    hdrShader->setBool("hdr", mHDR);
    hdrShader->setBool("bloom", bloomEnabled);
    hdrShader->setFloat("bloomStrength", bloomStrength);

    // HDR color
    mHDRFrameBuffer->getColorAttachment(0)->bind(PostProcessSlots::HDR);
    hdrShader->setInt("hdrBuffer", PostProcessSlots::HDR);

    // Bloom color
    mPingPongFBO[0]->getColorAttachment(0)->bind(PostProcessSlots::BLOOM);
    mPingPongFBO[1]->getColorAttachment(0)->bind(PostProcessSlots::BLOOM + 1);
    // Typically, we only need one final blurred result, so we just bind pingPongFBO[!horizontal].
    // For completeness, we could keep both, but only one is actually used in shading.

    hdrShader->setInt("bloomBuffer", PostProcessSlots::BLOOM);

    OpenGlUtil::drawQuad();
}

// ------------------------------------------------------
// Resource Binding
// ------------------------------------------------------

void SRender::bindSkyboxResources(std::shared_ptr<Shader>& shader)
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
    shader->setFloat("bloomThreshold", bloomThreshold);
}

void SRender::bindShadowMaps(std::shared_ptr<Shader>& shader)
{
    mDirectionalShadowMapBuffer->getDepthAttachment()->bind(ShadowSlots::DIRECTIONAL);
    shader->setInt("directionalShadowMap", ShadowSlots::DIRECTIONAL);

    mSpotShadowMapBuffer->getDepthAttachment()->bind(ShadowSlots::SPOT);
    shader->setInt("spotShadowMap", ShadowSlots::SPOT);

    mPointShadwMapBuffer->getDepthAttachment()->bind(ShadowSlots::POINT);
    shader->setInt("pointShadowMap", ShadowSlots::POINT);
}

// ------------------------------------------------------
// Drawing Helpers
// ------------------------------------------------------



void SRender::drawRenderList(const std::vector<RenderItem>& renderList, std::shared_ptr<Shader>& shader, bool bindMaterial) const
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
        ImGui::SliderFloat3("Position", glm::value_ptr(camera.Position), -25.0f, 25.0f);
        ImGui::SliderFloat("Yaw", &camera.Yaw, -180.0f, 180.0f);
        ImGui::SliderFloat("Pitch", &camera.Pitch, -89.0f, 89.0f);
        ImGui::SliderFloat("FOV", &camera.Fov, 1.0f, 120.0f);
        camera.updateCameraVectors();
        camera.updateProjectionMatrix();
    }

    if (ImGui::CollapsingHeader("Post-Processing"))
    {
        // HDR Settings
        ImGui::Checkbox("Enable HDR", &mHDR);
        ImGui::SliderFloat("Exposure", &mExposure, 0.0f, 5.0f);

        // Bloom Settings
        ImGui::Separator();
        ImGui::Checkbox("Enable Bloom", &bloomEnabled);
        if (bloomEnabled)
        {
            ImGui::SliderFloat("Bloom Threshold", &bloomThreshold, 0.0f, 1.0f);
            ImGui::SliderFloat("Bloom Strength", &bloomStrength, 0.0f, 2.0f);
            ImGui::SliderInt("Blur Passes", &blurPasses, 1, 20);
        }

        // SSAO Settings
        ImGui::Separator();
        ImGui::Checkbox("Enable SSAO", &ssaoEnabled);
        if (ssaoEnabled)
        {
            ImGui::SliderFloat("SSAO Radius", &mSSAORadius, 0.0f, 5.0f);
            ImGui::SliderFloat("SSAO Bias", &mSSAOBias, 0.0f, 0.5f);
            ImGui::SliderFloat("SSAO Power", &mSSAOPower, 0.0f, 5.0f);
			ImGui::SliderFloat("SSAO Blur Radius", &mSSAOBlurRadius, 0.0f, 5.0f);
			ImGui::SliderFloat("SSAO Blur Depth Threshold", &mSSAOBlurDepthThreshold, 0.0f, 1.0f);
			ImGui::SliderFloat("SSAO Blur Normal Threshold", &mSSAOBlurNormalThreshold, 0.0f, 1.0f);
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
            bool debugOutput = OpenGlUtil::isDebugOutputEnabled();
            if (ImGui::Checkbox("Debug Output", &debugOutput))
            {
                OpenGlUtil::enableDebugOutput(debugOutput);
            }

            bool profiling = OpenGlUtil::GPUTimer::isProfilingEnabled();
            if (ImGui::Checkbox("GPU Profiling", &profiling))
            {
                OpenGlUtil::GPUTimer::enableProfiling(profiling);
            }

            static bool breakOnError = true;
            if (ImGui::Checkbox("Break On Error", &breakOnError))
            {
                OpenGlUtil::setBreakOnError(breakOnError);
            }
        }
    }

    if (ImGui::CollapsingHeader("Scene Objects"))
    {
        auto& registry = mScene->mEnttRegistry;
        static int selectedEntityIndex = -1;
        std::vector<entt::entity> entities;
        std::vector<std::string> entityLabels;

        auto view = registry.view<CModel, CTransform>();
        for (auto entity : view)
        {
            entities.push_back(entity);
            auto& modelComp = view.get<CModel>(entity);
            entityLabels.push_back(modelComp.name + " (" + std::to_string(static_cast<uint32_t>(entity)) + ")");
        }

        std::vector<const char*> labelPointers;
        for (const auto& label : entityLabels)
        {
            labelPointers.push_back(label.c_str());
        }

        ImGui::ListBox("Select Object", &selectedEntityIndex, labelPointers.data(), static_cast<int>(labelPointers.size()), 4);

        if (selectedEntityIndex >= 0 && selectedEntityIndex < entities.size())
        {
            auto entity = entities[selectedEntityIndex];
            auto& transform = view.get<CTransform>(entity);

            ImGui::Separator();

            // Position Control
            ImGui::SliderFloat3("Position", glm::value_ptr(transform.position), -10.0f, 10.0f);

            // Rotation Control (Euler angles)
            glm::vec3 eulerDegrees = glm::degrees(glm::eulerAngles(transform.rotation));
            ImGui::SliderFloat3("Rotation", glm::value_ptr(eulerDegrees), -180.0f, 180.0f);
            transform.rotation = glm::quat(glm::radians(eulerDegrees));

            // Scale Control
            ImGui::SliderFloat3("Scale", glm::value_ptr(transform.scale), 0.0f, 10.0f);

            transform.dirty = true;
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
    // Performance Statistics Window
    ImGui::Begin("Performance Statistics");
    {
        ImGuiIO& io = ImGui::GetIO();

        // FPS and frame timing
        ImGui::Text("FPS: %.1f (%.2f ms/frame)", io.Framerate, 1000.0f / io.Framerate);
        ImGui::Separator();
        ImGui::Text("Culling Statistics:");
        int totalMeshes = mOpaqueRenderList.size() + mTransparentRenderList.size() + mCulledMeshes;
        ImGui::Text("Total Meshes: %d", totalMeshes);
        ImGui::Text("Visible Meshes: %d", mOpaqueRenderList.size() + mTransparentRenderList.size());
        ImGui::Text("Culled Meshes: %d", mCulledMeshes);
        float cullPercentage = (totalMeshes > 0) ? (float)mCulledMeshes / totalMeshes * 100.0f : 0.0f;
        ImGui::Text("Culling Percentage: %.1f%%", cullPercentage);

        // GPU Timings for each render pass
        if (OpenGlUtil::GPUTimer::isProfilingEnabled())
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
            auto displayTiming = [](const char* label, float time) {
                ImGui::Text("%s", label); ImGui::NextColumn();
                ImGui::Text("%.2f", time); ImGui::NextColumn();
                };

            // Display timings for main passes
            displayTiming("Light Data", OpenGlUtil::GPUTimer::getLastDuration("Light Data"));
            displayTiming("Shadow Pass", OpenGlUtil::GPUTimer::getLastDuration("Shadow Pass"));
            displayTiming("Depth Pass", OpenGlUtil::GPUTimer::getLastDuration("Depth Pass"));
            displayTiming("G-Buffer", OpenGlUtil::GPUTimer::getLastDuration("G-Buffer"));
            displayTiming("SSAO", OpenGlUtil::GPUTimer::getLastDuration("SSAO"));
            displayTiming("Deferred Lighting", OpenGlUtil::GPUTimer::getLastDuration("Deferred Lighting"));
            displayTiming("Post Processing", OpenGlUtil::GPUTimer::getLastDuration("Post Processing"));

            // Shadow sub-passes
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Shadow Sub-passes:"); ImGui::NextColumn();
            ImGui::NextColumn();
            displayTiming("  Directional Shadows", OpenGlUtil::GPUTimer::getLastDuration("Directional Shadows"));
            displayTiming("  Spot Shadows", OpenGlUtil::GPUTimer::getLastDuration("Spot Shadows"));
            displayTiming("  Point Shadows", OpenGlUtil::GPUTimer::getLastDuration("Point Shadows"));


            
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
        do {
            sample = glm::vec3(
                randomFloats(generator) * 2.0f - 1.0f,
                randomFloats(generator) * 2.0f - 1.0f,
                randomFloats(generator)  // Only positive z for hemisphere
            );
        } while (glm::length(sample) > 1.0f);  // Reject points outside unit sphere

        sample = glm::normalize(sample);

        // Scale samples s.t. they're more aligned to center of kernel
        float scale = (float)i / SSAO_KERNEL_SIZE;
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
    mSSAONoise = Texture2D::createTexture2D(noiseTexture, SSAO_NOISE_SIZE, SSAO_NOISE_SIZE);
}









