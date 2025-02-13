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

SRender::SRender(BaseScene* scene) : mScene(scene)
{
}

void SRender::init()
{
    glClearColor(0, 0, 0, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_CULL_FACE);

    glCullFace(GL_BACK);

    glFrontFace(GL_CCW);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);



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
    ImGui_ImplOpenGL3_Init("#version 400");

	mCameraUBO = UniformBuffer::createUniformBuffer(sizeof(CameraData), CAMERA_BINDING);
	mLightUBO = UniformBuffer::createUniformBuffer(sizeof(LightData), LIGHT_BINDING);


}

void SRender::render()

{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    OpenGlUtil::clearBackground();

    auto& registry = mScene->mEnttRegistry;

    const auto& view_matrix = mScene->mCurrentCamera.GetViewMatrix();
    const auto& proj_matrix = mScene->mCurrentCamera.GetProjectionMatrix();

	CameraData cameraData;
	cameraData.view = view_matrix;
	cameraData.projection = proj_matrix;
	cameraData.cameraPos = glm::vec4(mScene->mCurrentCamera.Position, 0.0f);

	mCameraUBO->setData(&cameraData, sizeof(CameraData));

	LightData lightData;
	buildDirectionalLights(lightData);
	buildPointLights(lightData);
	buildSpotLights(lightData);
	mLightUBO->setData(&lightData, sizeof(LightData));

    auto skybox = GameManager::mGraphicsManager->getEnvironmentMap("default");
    skybox->bindIrradiance(TEXTURE_UNIT_IRRADIANCE);
    skybox->bindPrefilter(TEXTURE_UNIT_PREFILTER);
    skybox->bindBRDFLUT(TEXTURE_UNIT_BRDFLUT);
    auto shader = GameManager::mGraphicsManager->getShader("PBR");
    shader->use();
	shader->setInt("irradianceMap", TEXTURE_UNIT_IRRADIANCE);
	shader->setInt("prefilterMap", TEXTURE_UNIT_PREFILTER);
	shader->setInt("brdfLUT", TEXTURE_UNIT_BRDFLUT);

    


	

    // Draw models
    drawModels(*shader);

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

        // Populate directional light data
        DirectionalLightData& dirLightData = lightData.directionalLights[numDirLights];
        dirLightData.direction = glm::vec4(light.direction, 0.0f);
        dirLightData.ambient = glm::vec4(light.ambient, 0.0f);
        dirLightData.diffuse = glm::vec4(light.diffuse, 0.0f);
        dirLightData.specular = glm::vec4(light.specular, 0.0f);

        numDirLights++;
    }
    lightData.counts.z = numDirLights; // counts.z stores the number of directional lights
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



void SRender::drawModels(const Shader& shader) const
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


#include <glm/gtc/type_ptr.hpp> // Add this include at the top of the file

void SRender::drawImGui() const
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

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}






