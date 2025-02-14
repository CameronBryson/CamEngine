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
#include "Texture2D.hpp"

SRender::SRender(BaseScene* scene) : mScene(scene)
{
}

void SRender::init()
{
    glClearColor(0, 0, 0, 0);


    glEnable(GL_DEPTH_TEST);




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
	mShadowMapBuffer = FrameBuffer::createFrameBuffer(mShadowMapWidth, mShadowMapHeight, { { FrameBufferAttachmentType::Depth, FrameBufferTextureFormat::Depth32F } });
    if (!mShadowMapBuffer->isComplete())
    {
        throw std::runtime_error("Shadow map framebuffer incomplete!");
    }

    // Verify the depth texture was created
    if (!mShadowMapBuffer->getDepthAttachment())
    {
        throw std::runtime_error("Shadow map depth attachment missing!");
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

	auto shadowShader = GameManager::mGraphicsManager->getShader("ShadowMap");
    shadowShader->use();
    shadowShader->setMat4("lightSpaceMatrix", lightData.directionalLights[0].lightSpaceMatrix );
	mShadowMapBuffer->setViewport(0, 0, mShadowMapWidth, mShadowMapHeight);
	mShadowMapBuffer->bind();
	mShadowMapBuffer->clear(GL_DEPTH_BUFFER_BIT);
	drawModelsShader(shadowShader);
	mShadowMapBuffer->unbind();

    int width, height;
    glfwGetFramebufferSize(GameManager::get_glfw_window(), &width, &height);
    glViewport(0, 0, width, height);

	if (mShowShadowMap)
	{
        auto debugShader = GameManager::mGraphicsManager->getShader("Debug");
		mShadowMapBuffer->getDepthAttachment()->bind(TEXTURE_UNIT_SHADOW);
		debugShader->use();
		debugShader->setInt("depthMap", TEXTURE_UNIT_SHADOW);
		drawModelsShader(debugShader);
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

	mShadowMapBuffer->getDepthAttachment()->bind(TEXTURE_UNIT_SHADOW);
	pbrShader->setInt("directionalShadowMap", TEXTURE_UNIT_SHADOW);


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
		// Populate spot light data
		SpotLightData& spotLightData = lightData.spotLights[numSpotLights];
		spotLightData.position = glm::vec4(light.position, 0.0f);
		spotLightData.direction = glm::vec4(light.direction, 0.0f);
		spotLightData.ambient = glm::vec4(light.ambient, 0.0f);
		spotLightData.diffuse = glm::vec4(light.diffuse, 0.0f);
		spotLightData.specular = glm::vec4(light.specular, 0.0f);
		spotLightData.attenuation = glm::vec4(light.constant, light.linear, light.quadratic, 0.0f);
		spotLightData.cutoffs = glm::vec4(light.innerCutoff, light.outerCutoff, 0.0f, 0.0f);
		numSpotLights++;

	}
	lightData.counts.y = numSpotLights; // counts.y stores the number of spot lights
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



#include <glm/gtc/type_ptr.hpp> // Add this include at the top of the file

void SRender::drawImGui()
{
    auto& camera = mScene->mCurrentCamera;
    auto& registry = mScene->mEnttRegistry;

    // Existing Camera Controls
    ImGui::Begin("Camera Controls");
    ImGui::Text("Adjust the camera parameters:");
    ImGui::SliderFloat3("Position", glm::value_ptr(camera.Position), -1000.0f, 1000.0f);
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
            ImGui::SliderFloat3("Position", glm::value_ptr(light.position), -1000.0f, 1000.0f);
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
            ImGui::SliderFloat3("Position", glm::value_ptr(light.position), -1000.0f, 1000.0f);
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
        ImGui::SliderFloat3("Position", glm::value_ptr(transform.position), -1000.0f, 1000.0f);

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

    // Add Debug Controls
    ImGui::Begin("Debug Controls");
    ImGui::Checkbox("Show Shadow Debug View", &mShowShadowMap);
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








