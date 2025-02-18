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




    // Retrieve window dimensions
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
    ImGui_ImplOpenGL3_Init("#version 450");

	mCameraUBO = UniformBuffer::createUniformBuffer(sizeof(CameraData), CAMERA_BINDING);
	mLightUBO = UniformBuffer::createUniformBuffer(sizeof(LightData), LIGHT_BINDING);
	mDirectionalShadowMapBuffer = FrameBuffer::createFrameBuffer(mShadowMapWidth, mShadowMapHeight, { { FrameBufferAttachmentType::Depth, FrameBufferTextureFormat::Depth32F } });
    if (!mDirectionalShadowMapBuffer->isComplete())
    {
        throw std::runtime_error("Shadow map framebuffer incomplete!");
    }
    // Verify the depth texture was created
    if (!mDirectionalShadowMapBuffer->getDepthAttachment())
    {
        throw std::runtime_error("Shadow map depth attachment missing!");
    }
	mSpotShadowMapBuffer = FrameBuffer::createFrameBuffer(mShadowMapWidth, mShadowMapHeight, { { FrameBufferAttachmentType::Depth, FrameBufferTextureFormat::Depth32F } });
	if (!mSpotShadowMapBuffer->isComplete())
	{
		throw std::runtime_error("Shadow map framebuffer incomplete!");
	}
	if (!mSpotShadowMapBuffer->getDepthAttachment())
	{
		throw std::runtime_error("Shadow map depth attachment missing!");
	}
	mPointShadwMapBuffer = FrameBuffer::createFrameBuffer(mShadowMapWidth, mShadowMapHeight, { { FrameBufferAttachmentType::DepthCubemap, FrameBufferTextureFormat::Depth32F } });
	if (!mPointShadwMapBuffer->isComplete())
	{
		throw std::runtime_error("Shadow map framebuffer incomplete!");
	}
    calculateSceneBounds();

}

void SRender::render()

{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    OpenGlUtil::clearBackground();


    auto& registry = mScene->mEnttRegistry;
    LightData lightData;
    buildDirectionalLights(lightData);
    buildPointLights(lightData);
    buildSpotLights(lightData);

    mLightUBO->setData(&lightData, sizeof(LightData));
    


    const auto& view_matrix = mScene->mCurrentCamera.GetViewMatrix();
    const auto& proj_matrix = mScene->mCurrentCamera.GetProjectionMatrix();

	CameraData cameraData;
	cameraData.view = view_matrix;
	cameraData.projection = proj_matrix;
	cameraData.cameraPos = glm::vec4(mScene->mCurrentCamera.Position, 0.0f);

	mCameraUBO->setData(&cameraData, sizeof(CameraData));

	auto shadowMapShader = GameManager::mGraphicsManager->getShader("ShadowMap");
	auto pointShadowMapShader = GameManager::mGraphicsManager->getShader("PointShadowMap");

    shadowMapShader->use();
    shadowMapShader->setMat4("lightSpaceMatrix", lightData.directionalLights[0].lightSpaceMatrix );
	mDirectionalShadowMapBuffer->setViewport(0, 0, mShadowMapWidth, mShadowMapHeight);
	mDirectionalShadowMapBuffer->bind();
	mDirectionalShadowMapBuffer->clear(GL_DEPTH_BUFFER_BIT);
	drawModelsShader(shadowMapShader);
	mDirectionalShadowMapBuffer->unbind();

    shadowMapShader->use();
	shadowMapShader->setMat4("lightSpaceMatrix", lightData.spotLights[0].lightSpaceMatrix);
	mSpotShadowMapBuffer->setViewport(0, 0, mShadowMapWidth, mShadowMapHeight);
	mSpotShadowMapBuffer->bind();
	mSpotShadowMapBuffer->clear(GL_DEPTH_BUFFER_BIT);
	drawModelsShader(shadowMapShader);
	mSpotShadowMapBuffer->unbind();

	pointShadowMapShader->use();
	pointShadowMapShader->setVec3("lightPos", lightData.pointLights[0].position);
	pointShadowMapShader->setFloat("far_plane", farPlane);
    for (int i = 0; i < 6; i++)
    {

		pointShadowMapShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", lightData.pointLights[0].shadowMatrices[i]);
    }
	mPointShadwMapBuffer->setViewport(0, 0, mShadowMapWidth, mShadowMapHeight);
	mPointShadwMapBuffer->bind();
	mPointShadwMapBuffer->clear(GL_DEPTH_BUFFER_BIT);
	drawModelsShader(pointShadowMapShader);
	mPointShadwMapBuffer->unbind();





    int width, height;
    glfwGetFramebufferSize(GameManager::get_glfw_window(), &width, &height);
    glViewport(0, 0, width, height);

    if (mShowShadowMap)
    {
        if (mShowShadowMap)
        {
            auto debugShader = GameManager::mGraphicsManager->getShader("Debug");
            debugShader->use();

            const auto& lightSpaceMatrix = lightData.directionalLights[0].lightSpaceMatrix;
            debugShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
            debugShader->setInt("depthMap", TEXTURE_UNIT_DIRECTIONAL_SHADOW);

            mDirectionalShadowMapBuffer->getDepthAttachment()->bind(TEXTURE_UNIT_DIRECTIONAL_SHADOW);

            drawModelsShader(debugShader);
        }

        drawImGui();
        return;
    }

    auto pbrShader = GameManager::mGraphicsManager->getShader("PBR");

    
    pbrShader->use();

    auto skybox = GameManager::mGraphicsManager->getEnvironmentMap("default");
    skybox->bindIrradiance(TEXTURE_UNIT_IRRADIANCE);
    skybox->bindPrefilter(TEXTURE_UNIT_PREFILTER);
    skybox->bindBRDFLUT(TEXTURE_UNIT_BRDFLUT);
    
	pbrShader->setInt("irradianceMap", TEXTURE_UNIT_IRRADIANCE);
	pbrShader->setInt("prefilterMap", TEXTURE_UNIT_PREFILTER);
	pbrShader->setInt("brdfLUT", TEXTURE_UNIT_BRDFLUT);
    

	mDirectionalShadowMapBuffer->getDepthAttachment()->bind(TEXTURE_UNIT_DIRECTIONAL_SHADOW);
	pbrShader->setInt("directionalShadowMap", TEXTURE_UNIT_DIRECTIONAL_SHADOW);

	mSpotShadowMapBuffer->getDepthAttachment()->bind(TEXTURE_UNIT_SPOT_SHADOW);
	pbrShader->setInt("spotShadowMap", TEXTURE_UNIT_SPOT_SHADOW);

	mPointShadwMapBuffer->getDepthAttachment()->bind(TEXTURE_UNIT_POINT_SHADOW);
	pbrShader->setInt("pointShadowMap", TEXTURE_UNIT_POINT_SHADOW);
    pbrShader->setFloat("farPlane", farPlane);
	pbrShader->setBool("enableShadows", mEnableShadows);


    // Draw models
    drawModels();

    auto skyboxShader = GameManager::mGraphicsManager->getShader("Skybox");


    skyboxShader->use();
    glm::mat4 view_no_translation = glm::mat4(glm::mat3(view_matrix));
    skyboxShader->setMat4("view", view_no_translation);
    skyboxShader->setMat4("projection", proj_matrix);

	skybox->drawSkybox(skyboxShader);

    drawImGui();
    
}

void SRender::shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void SRender::buildDirectionalLights(LightData& lightData)
{
    auto dirLightView = mScene->mEnttRegistry.view<CDirectionalLight>();
    int numDirLights = 0;
    for (auto entity : dirLightView)
    {
        if (numDirLights >= MAX_DIRECTIONAL_LIGHTS)
            break;
        auto& light = dirLightView.get<CDirectionalLight>(entity);

        // Fill in the light's basic properties.
        DirectionalLightData& data = lightData.directionalLights[numDirLights];
        data.direction = glm::vec4(light.direction, 0.0f);
        data.ambient = glm::vec4(light.ambient, 0.0f);
        data.diffuse = glm::vec4(light.diffuse, 0.0f);
        data.specular = glm::vec4(light.specular, 0.0f);

        // Compute the light view matrix.
        // We position the light far enough along its direction so that the scene is fully in view.
        glm::vec3 lightDir = glm::normalize(glm::vec3(light.direction));
        glm::vec3 lightPos = mSceneBounds.center - lightDir * (mSceneBounds.radius * 2.0f);
        glm::vec3 up = fabs(glm::dot(lightDir, glm::vec3(0, 1, 0))) > 0.99f
            ? glm::vec3(1, 0, 0)
            : glm::vec3(0, 1, 0);
        glm::mat4 lightView = glm::lookAt(lightPos, mSceneBounds.center, up);

        // Now transform the eight corners of the scene bounding box into light space.
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

        glm::vec3 lightSpaceMin(std::numeric_limits<float>::max());
        glm::vec3 lightSpaceMax(std::numeric_limits<float>::lowest());

        for (int i = 0; i < 8; ++i) {
            glm::vec3 cornerLS = glm::vec3(lightView * glm::vec4(boxCorners[i], 1.0f));
            lightSpaceMin = glm::min(lightSpaceMin, cornerLS);
            lightSpaceMax = glm::max(lightSpaceMax, cornerLS);
        }

        // Optionally add a small padding to avoid clipping.
        float padding = glm::length(lightSpaceMax - lightSpaceMin) * 0.05f;
        lightSpaceMin -= glm::vec3(padding);
        lightSpaceMax += glm::vec3(padding);

        // Build an orthographic projection using the tight bounds.
        glm::mat4 lightProj = glm::ortho(
            lightSpaceMin.x, lightSpaceMax.x,
            lightSpaceMin.y, lightSpaceMax.y,
            -lightSpaceMax.z, -lightSpaceMin.z
        );

        // Combine projection and view to form the final light space matrix.
        data.lightSpaceMatrix = lightProj * lightView;

        numDirLights++;
    }
    lightData.counts.z = numDirLights;
}


void SRender::buildPointLights(LightData& lightData)
{
    auto pointLightView = mScene->mEnttRegistry.view<CPointLight>();
    int numPointLights = 0;
    glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);
    for (auto entity : pointLightView)
    {
        if (numPointLights >= MAX_POINT_LIGHTS)
            break;

        auto& light = pointLightView.get<CPointLight>(entity);

        // Populate point light data
        PointLightData& pointLightData = lightData.pointLights[numPointLights];
        pointLightData.position = glm::vec4(light.position, 0.0f);
        pointLightData.ambient = glm::vec4(light.ambient, 0.0f);
        pointLightData.diffuse = glm::vec4(light.diffuse, 0.0f);
        pointLightData.specular = glm::vec4(light.specular, 0.0f);
        pointLightData.attenuation = glm::vec4(light.constant, light.linear, light.quadratic, 0.0f);
		pointLightData.shadowMatrices[0] = shadowProjection * glm::lookAt(light.position, light.position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		pointLightData.shadowMatrices[1] = shadowProjection * glm::lookAt(light.position, light.position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		pointLightData.shadowMatrices[2] = shadowProjection * glm::lookAt(light.position, light.position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		pointLightData.shadowMatrices[3] = shadowProjection * glm::lookAt(light.position, light.position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		pointLightData.shadowMatrices[4] = shadowProjection * glm::lookAt(light.position, light.position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		pointLightData.shadowMatrices[5] = shadowProjection * glm::lookAt(light.position, light.position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        numPointLights++;
    }
    lightData.counts.x = numPointLights; // counts.x stores the number of point lights
}

void SRender::buildSpotLights(LightData& lightData)
{
    auto spotLightView = mScene->mEnttRegistry.view<CSpotLight>();
    int numSpotLights = 0;
    for (auto entity : spotLightView)
    {
        if (numSpotLights >= MAX_SPOT_LIGHTS)
            break;
        auto& light = spotLightView.get<CSpotLight>(entity);

        // Populate basic spot light data
        SpotLightData& spotLightData = lightData.spotLights[numSpotLights];
        spotLightData.position = glm::vec4(light.position, 0.0f);
        spotLightData.direction = glm::vec4(light.direction, 0.0f);
        spotLightData.ambient = glm::vec4(light.ambient, 0.0f);
        spotLightData.diffuse = glm::vec4(light.diffuse, 0.0f);
        spotLightData.specular = glm::vec4(light.specular, 0.0f);
        spotLightData.attenuation = glm::vec4(light.constant, light.linear, light.quadratic, 0.0f);
        spotLightData.cutoffs = glm::vec4(light.innerCutoff, light.outerCutoff, 0.0f, 0.0f);

        // 1. Calculate the light's view matrix
        glm::vec3 lightDir = glm::normalize(light.direction);

        // Handle up vector similar to directional light
        glm::vec3 up = fabs(glm::dot(lightDir, glm::vec3(0, 1, 0))) > 0.99f
            ? glm::vec3(1, 0, 0)
            : glm::vec3(0, 1, 0);

        glm::mat4 lightView = glm::lookAt(
            light.position,                // Eye position (light position)
            light.position + lightDir,     // Look target
            up                            // Up vector
        );

        // 2. Transform scene bounds to light space to find depth range
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

        // 3. Find min/max depth in light space
        float minDepth = std::numeric_limits<float>::max();
        float maxDepth = std::numeric_limits<float>::lowest();

        for (const auto& corner : boxCorners)
        {
            // Transform to light space
            glm::vec4 lightSpacePos = lightView * glm::vec4(corner, 1.0f);

            // Get the distance from light to corner
            float depth = -lightSpacePos.z; // Negative because OpenGL looks down -Z
            minDepth = std::min(minDepth, depth);
            maxDepth = std::max(maxDepth, depth);
        }

        // 4. Add padding to avoid clipping
        float depthPadding = (maxDepth - minDepth) * 0.05f;
        float nearPlane = std::max(0.1f, minDepth - depthPadding);
        float farPlane = maxDepth + depthPadding;

        // 5. Calculate FOV from outer cutoff (which is stored as cos(angle))
        float spotAngle = glm::acos(light.outerCutoff);

        // 6. Create perspective projection
        // Note: We use spotAngle * 2 because the cutoff is half the total FOV
        glm::mat4 lightProjection = glm::perspective(
            spotAngle * 2.0f,    // Full FOV
            1.0f,                // Aspect ratio (1.0 for square shadow map)
            nearPlane,           // Near plane based on scene bounds
            farPlane            // Far plane based on scene bounds
        );

        // 7. Combine view and projection
        spotLightData.lightSpaceMatrix = lightProjection * lightView;

        numSpotLights++;
    }
    lightData.counts.y = numSpotLights;
}






void SRender::drawModels() const
{
    auto& registry = mScene->mEnttRegistry;

    // View for entities with CModel and CTransform components
    auto modelView = registry.view<CModel, CTransform>();

    for (auto entity : modelView)
    {
        auto& modelComp = modelView.get<CModel>(entity);
        const auto& transform = modelView.get<CTransform>(entity);

        glm::mat4 model_matrix = transform.model_matrix;

        GameManager::mGraphicsManager->getModel(modelComp.name)->draw(model_matrix);
    }
}

void SRender::drawModelsShader(std::shared_ptr<Shader>& shader) const
{
	auto& registry = mScene->mEnttRegistry;
	// View for entities with CModel and CTransform components
	auto modelView = registry.view<CModel, CTransform>();
	for (auto entity : modelView)
	{
		auto& modelComp = modelView.get<CModel>(entity);
		const auto& transform = modelView.get<CTransform>(entity);
		glm::mat4 model_matrix = transform.model_matrix;
		GameManager::mGraphicsManager->getModel(modelComp.name)->drawShadow(shader, model_matrix);
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
    ImGui::SliderFloat("FOV", &camera.FOV, 1.0f, 120.0f);
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
    ImGui::Checkbox("Show Directional Shadow Debug", &mShowShadowMap);
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



    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SRender::calculateSceneBounds()
{
    auto& registry = mScene->mEnttRegistry;
    auto view = registry.view<CModel, CTransform>();
    glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

    bool foundAny = false;

    for (auto entity : view)
    {
        auto& modelTransform = view.get<CTransform>(entity);
        auto& model = GameManager::mGraphicsManager->getModel(view.get<CModel>(entity).name);
        auto& meshes = model->getMeshes();

        for (const auto& meshInstance : meshes)
        {
            auto& mesh = meshInstance.mesh;
            auto& localTransform = meshInstance.localTransform;
            auto& vertices = mesh->getVertices();

            // Combined transform: entity transform * mesh local transform
            glm::mat4 finalTransform = modelTransform.model_matrix * localTransform;

            for (const auto& vertex : vertices)
            {
                // Transform vertex to world space
                glm::vec4 worldPos = finalTransform * glm::vec4(vertex.position, 1.0f);
                glm::vec3 transformedPos = glm::vec3(worldPos);

                min = glm::min(min, transformedPos);
                max = glm::max(max, transformedPos);
                foundAny = true;
            }
        }
    }

    if (!foundAny)
    {
        // Default bounds if no vertices found
        min = glm::vec3(-1.0f);
        max = glm::vec3(1.0f);
    }

    // Update scene bounds
    mSceneBounds.min = min;
    mSceneBounds.max = max;
    mSceneBounds.center = (max + min) * 0.5f;
    mSceneBounds.radius = glm::length(max - mSceneBounds.center);

    // Add some padding
    float padding = mSceneBounds.radius * 0.1f; // 10% padding
    mSceneBounds.min -= glm::vec3(padding);
    mSceneBounds.max += glm::vec3(padding);
    mSceneBounds.radius *= 1.1f;
}








