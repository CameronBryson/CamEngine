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

    ImGui::Begin("Camera Controls");
    ImGui::Text("Adjust the camera parameters:");

    // Position controls
    ImGui::SliderFloat3("Position", glm::value_ptr(camera.Position), -100.0f, 100.0f);

    // Orientation controls (Yaw and Pitch)
    ImGui::SliderFloat("Yaw", &camera.Yaw, -180.0f, 180.0f);
    ImGui::SliderFloat("Pitch", &camera.Pitch, -89.0f, 89.0f);

    // Zoom control
    ImGui::SliderFloat("FOV", &camera.FOV, 1.0f, 120.0f);

    // Update camera vectors after changes
    camera.updateCameraVectors();
    camera.updateProjectionMatrix();

    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}




