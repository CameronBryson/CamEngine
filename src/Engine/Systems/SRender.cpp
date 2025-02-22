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
SRender::SRender(BaseScene* scene) : mScene(scene)
{
}

void SRender::init()
{
    initImGui();
    initFramebuffers();
}

void SRender::lateInit()
{
    calculateSceneBounds();
}

void SRender::render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Build and upload light data
    LightData lightData;
    buildLightData(lightData);
    updateCameraUniforms();

    // Execute the main render pipeline
    shadowPass(lightData);
    geometryPass();
    lightingPass();
    postProcessPass();

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

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void SRender::initFramebuffers()
{
    // Create uniform buffers
    mCameraUBO = UniformBuffer::createUniformBuffer(sizeof(CameraData), CAMERA_BINDING);
    mLightUBO = UniformBuffer::createUniformBuffer(sizeof(LightData), LIGHT_BINDING);

    // Directional & Spot shadow map FBO
    auto depthAttachment =
        std::vector<FrameBufferAttachmentSpecification>{
            { FrameBufferAttachmentType::Depth, FrameBufferTextureFormat::Depth32F }
    };

    mDirectionalShadowMapBuffer = FrameBuffer::createFrameBuffer(mShadowMapWidth, mShadowMapHeight, depthAttachment);
    mDirectionalShadowMapBuffer->getDepthAttachment()->setShadowSamplerParameters();
    if (!mDirectionalShadowMapBuffer->isComplete() || !mDirectionalShadowMapBuffer->getDepthAttachment())
        throw std::runtime_error("Directional shadow map framebuffer setup failed!");

    mSpotShadowMapBuffer = FrameBuffer::createFrameBuffer(mShadowMapWidth, mShadowMapHeight, depthAttachment);
    mSpotShadowMapBuffer->getDepthAttachment()->setShadowSamplerParameters();
    if (!mSpotShadowMapBuffer->isComplete() || !mSpotShadowMapBuffer->getDepthAttachment())
        throw std::runtime_error("Spot shadow map framebuffer setup failed!");

    // Point shadow map FBO (cubemap)
    auto depthCubemapAttachment =
        std::vector<FrameBufferAttachmentSpecification>{
            { FrameBufferAttachmentType::DepthCubemap, FrameBufferTextureFormat::Depth32F }
    };

    mPointShadwMapBuffer = FrameBuffer::createFrameBuffer(mShadowMapWidth, mShadowMapHeight, depthCubemapAttachment);
    mPointShadwMapBuffer->getDepthAttachment()->setShadowSamplerParameters();
    if (!mPointShadwMapBuffer->isComplete())
        throw std::runtime_error("Point shadow map framebuffer setup failed!");

    // HDR FBO
    std::vector<FrameBufferAttachmentSpecification> hdrAttachments = {
        { FrameBufferAttachmentType::Color,  FrameBufferTextureFormat::RGBA16F },
        { FrameBufferAttachmentType::Color,  FrameBufferTextureFormat::RGBA16F },
        { FrameBufferAttachmentType::Depth,  FrameBufferTextureFormat::Depth32F }
    };

    mHDRFrameBuffer = FrameBuffer::createFrameBuffer(settings::window_width, settings::window_height, hdrAttachments);
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

void SRender::shadowPass(const LightData& lightData)
{
    // Front-face culling for shadow rendering
    GL_CHECK(glCullFace(GL_FRONT));

    // Render directional, spot, and point shadows
    renderDirectionalShadows(lightData);
    renderSpotShadows(lightData);
    renderPointShadows(lightData);

    // Restore back-face culling
    GL_CHECK(glCullFace(GL_BACK));

    // Update light UBO after shadow pass
    mLightUBO->setData(&lightData, sizeof(LightData));
}

void SRender::geometryPass()
{
    int width, height;
    glfwGetFramebufferSize(GameManager::get_glfw_window(), &width, &height);

    GL_CHECK(glViewport(0, 0, width, height));
    mHDRFrameBuffer->bind();
    mHDRFrameBuffer->setViewport(0, 0, width, height);
    mHDRFrameBuffer->setDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    auto pbrShader = GameManager::mGraphicsManager->getShader("PBR");
    pbrShader->use();

    bindSkyboxResources(pbrShader);  // Also binds shadow maps
    drawModels();                    // Draw opaque geometry
    // (If there’s additional translucent geometry, it could be handled separately.)
}

void SRender::lightingPass()
{
    // Renders the skybox (and any additional lighting passes if needed)
    auto skyboxShader = GameManager::mGraphicsManager->getShader("Skybox");
    skyboxShader->use();

    const auto& viewMatrix = mScene->mCurrentCamera.GetViewMatrix();
    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(viewMatrix));

    skyboxShader->setMat4("view", viewNoTranslation);
    skyboxShader->setMat4("projection", mScene->mCurrentCamera.GetProjectionMatrix());

    auto skybox = GameManager::mGraphicsManager->getEnvironmentMap("default");
    skybox->drawSkybox(skyboxShader);

    // Unbind HDR FBO so we can do post-processing
    mHDRFrameBuffer->unbind();
}

void SRender::postProcessPass()
{
    bloomPass();
    hdrPass();
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

        drawModelsShader(shadowMapShader);

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

        drawModelsShader(shadowMapShader);

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

        drawModelsShader(pointShadowMapShader);

        mPointShadwMapBuffer->unbind();
    }
}

// ------------------------------------------------------
// Post-Processing
// ------------------------------------------------------

void SRender::bloomPass()
{
    int width, height;
    glfwGetFramebufferSize(GameManager::get_glfw_window(), &width, &height);

    auto blurShader = GameManager::mGraphicsManager->getShader("Blur");
    blurShader->use();

    bool horizontal = true;
    bool firstIteration = true;

    // Perform multiple Gaussian blur passes, alternating ping-pong FBO
    for (int i = 0; i < blurPasses; i++)
    {
        mPingPongFBO[horizontal]->bind();
        mPingPongFBO[horizontal]->setViewport(0, 0, width, height);
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

void SRender::drawModels() const
{
    // Draw each entity's model
    auto& registry = mScene->mEnttRegistry;
    auto modelView = registry.view<CModel, CTransform>();

    for (auto entity : modelView)
    {
        const auto& modelComp = modelView.get<CModel>(entity);
        const auto& transform = modelView.get<CTransform>(entity);

        glm::mat4 modelMatrix = transform.model_matrix;
        GameManager::mGraphicsManager->getModel(modelComp.name)->draw(modelMatrix);
    }
}

void SRender::drawModelsShader(std::shared_ptr<Shader>& shader) const
{
    // Draw geometry with a specific shader (e.g. shadow pass)
    auto& registry = mScene->mEnttRegistry;
    auto modelView = registry.view<CModel, CTransform>();

    for (auto entity : modelView)
    {
        const auto& modelComp = modelView.get<CModel>(entity);
        const auto& transform = modelView.get<CTransform>(entity);

        glm::mat4 modelMatrix = transform.model_matrix;
        GameManager::mGraphicsManager->getModel(modelComp.name)->drawShadow(shader, modelMatrix);
    }
}

void SRender::drawImGui()
{
    auto& camera = mScene->mCurrentCamera;
    auto& registry = mScene->mEnttRegistry;

    // Existing Camera Controls
    ImGui::Begin("Camera Controls");
    ImGui::Text("Adjust the camera parameters:");
    ImGui::SliderFloat3("Position", glm::value_ptr(camera.Position), -25.0f, 25.0f);
    ImGui::SliderFloat("Yaw", &camera.Yaw, -180.0f, 180.0f);
    ImGui::SliderFloat("Pitch", &camera.Pitch, -89.0f, 89.0f);
    ImGui::SliderFloat("FOV", &camera.Fov, 1.0f, 120.0f);
    camera.updateCameraVectors();
    camera.updateProjectionMatrix();
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::End();

    // New window for light controls
    ImGui::Begin("Light Controls");
    ImGui::Text("Adjust the lighting parameters:");

    // Directional Lights
    auto dirLightView = registry.view<CDirectionalLight>();
    int dirLightIndex = 0;
    for (auto entity : dirLightView)
    {
        auto& light = dirLightView.get<CDirectionalLight>(entity);
        ImGui::PushID(dirLightIndex);
        if (ImGui::TreeNode("Directional Light"))
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

    // Point Lights
    auto pointLightView = registry.view<CPointLight>();
    int pointLightIndex = 0;
    for (auto entity : pointLightView)
    {
        auto& light = pointLightView.get<CPointLight>(entity);
        ImGui::PushID(pointLightIndex);
        if (ImGui::TreeNode("Point Light"))
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

    // Spot Lights
    auto spotLightView = registry.view<CSpotLight>();
    int spotLightIndex = 0;
    for (auto entity : spotLightView)
    {
        auto& light = spotLightView.get<CSpotLight>(entity);
        ImGui::PushID(spotLightIndex);
        if (ImGui::TreeNode("Spot Light"))
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

    ImGui::End();
    ImGui::Begin("Object Controls");
    ImGui::Text("Select an object to manipulate:");

    // Create a list of entities with CModel and CTransform components
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

    // Convert labels to const char* array for ImGui
    std::vector<const char*> labelPointers;
    for (const auto& label : entityLabels)
    {
        labelPointers.push_back(label.c_str());
    }

    // Entity selection dropdown
    ImGui::ListBox("Entities", &selectedEntityIndex, labelPointers.data(), static_cast<int>(labelPointers.size()), 5);

    if (selectedEntityIndex >= 0 && selectedEntityIndex < entities.size())
    {
        auto entity = entities[selectedEntityIndex];
        auto& transform = view.get<CTransform>(entity);

        ImGui::Separator();
        ImGui::Text("Transform Controls:");

        // Position Control
        ImGui::SliderFloat3("Position", glm::value_ptr(transform.position), -10.0f, 10.0f);

        // Rotation Control (Euler angles)
        glm::vec3 eulerDegrees = glm::degrees(glm::eulerAngles(transform.rotation));
        ImGui::SliderFloat3("Rotation (Degrees)", glm::value_ptr(eulerDegrees), -180.0f, 180.0f);
        transform.rotation = glm::quat(glm::radians(eulerDegrees));

        // Scale Control
        ImGui::SliderFloat3("Scale", glm::value_ptr(transform.scale), 0.0f, 10.0f);

        // Mark transform as dirty to update model matrix
        transform.dirty = true;
    }
    ImGui::End();

    // In SRender::drawImGui()
    ImGui::Begin("Debug Controls");
    ImGui::Checkbox("Show Shadows", &mEnableShadows);
    ImGui::End();


    // Add Scene Bounds Controls
    ImGui::Begin("Scene Bounds Controls");
    ImGui::Text("Scene Bounds Parameters:");

    // Center control
    ImGui::Text("Center:");
    if (ImGui::SliderFloat3("##Center", glm::value_ptr(mSceneBounds.center), -1000.0f, 1000.0f))
    {
        // Recalculate bounds when center changes
        glm::vec3 extents = (mSceneBounds.max - mSceneBounds.min) * 0.5f;
        mSceneBounds.min = mSceneBounds.center - extents;
        mSceneBounds.max = mSceneBounds.center + extents;
    }

    // Radius control
    if (ImGui::SliderFloat("Radius", &mSceneBounds.radius, 0.1f, 1000.0f))
    {
        // Update min/max based on radius change
        glm::vec3 extents = glm::vec3(mSceneBounds.radius);
        mSceneBounds.min = mSceneBounds.center - extents;
        mSceneBounds.max = mSceneBounds.center + extents;
    }

    // Min/Max bounds
    if (ImGui::TreeNode("Advanced"))
    {
        ImGui::Text("Min/Max Bounds:");
        bool boundsChanged = false;
        boundsChanged |= ImGui::SliderFloat3("Min", glm::value_ptr(mSceneBounds.min), -1000.0f, 1000.0f);
        boundsChanged |= ImGui::SliderFloat3("Max", glm::value_ptr(mSceneBounds.max), -1000.0f, 1000.0f);

        if (boundsChanged)
        {
            // Update center and radius when min/max change
            mSceneBounds.center = (mSceneBounds.max + mSceneBounds.min) * 0.5f;
            mSceneBounds.radius = glm::length(mSceneBounds.max - mSceneBounds.center);
        }

        ImGui::TreePop();
    }

    // Add buttons for common operations
    if (ImGui::Button("Recalculate Bounds"))
    {
        calculateSceneBounds();
    }

    ImGui::SameLine();

    if (ImGui::Button("Add 10% Padding"))
    {
        float padding = mSceneBounds.radius * 0.1f;
        mSceneBounds.min -= glm::vec3(padding);
        mSceneBounds.max += glm::vec3(padding);
        mSceneBounds.radius *= 1.1f;
    }
    if (ImGui::Button("Remove 10% Padding"))
    {
        float padding = mSceneBounds.radius * 0.1f;
        mSceneBounds.min += glm::vec3(padding);
        mSceneBounds.max -= glm::vec3(padding);
        mSceneBounds.radius /= 1.1f;
    }

    // Display current values
    ImGui::Separator();
    ImGui::Text("Current Values:");
    ImGui::Text("Center: (%.2f, %.2f, %.2f)",
        mSceneBounds.center.x, mSceneBounds.center.y, mSceneBounds.center.z);
    ImGui::Text("Radius: %.2f", mSceneBounds.radius);
    ImGui::Text("Min: (%.2f, %.2f, %.2f)",
        mSceneBounds.min.x, mSceneBounds.min.y, mSceneBounds.min.z);
    ImGui::Text("Max: (%.2f, %.2f, %.2f)",
        mSceneBounds.max.x, mSceneBounds.max.y, mSceneBounds.max.z);

    ImGui::End();

    // In drawImGui()
    ImGui::Begin("Post-Processing");
    ImGui::Checkbox("Enable HDR", &mHDR);
    ImGui::SliderFloat("Exposure", &mExposure, 0.0f, 5.0f);
    ImGui::Separator();
    ImGui::Checkbox("Enable Bloom", &bloomEnabled);
    ImGui::SliderFloat("Bloom Threshold", &bloomThreshold, 0.0f, 1.0f);
    ImGui::SliderFloat("Bloom Strength", &bloomStrength, 0.0f, 2.0f);
    ImGui::SliderInt("Blur Passes", &blurPasses, 1, 20);
    ImGui::End();





    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}








