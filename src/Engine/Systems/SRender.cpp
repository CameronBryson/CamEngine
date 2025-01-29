#include "pch.hpp"
#include "SRender.hpp"

#include "Engine/Components.hpp"
#include "Engine/Util/EngineUtil.hpp"
#include "Engine/Util/platform.hpp"

#include "Engine/Util/OpenGLUtil.hpp"
#include "Engine/Managers/GameManager.hpp"
#include "Engine/Managers/GraphicsManager.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Model.hpp"
#include "Engine/Graphics/Mesh.hpp"
#include "Engine/Graphics/Font.hpp"
#include <string>
#include "Engine/Base/BaseScene.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "platform.hpp"
#include "edyn/edyn.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include <GameSettings.hpp>

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

    // Retrieve window dimensions
    GLFWwindow* window = GameManager::get_glfw_window();
    glfwGetFramebufferSize(window, reinterpret_cast<int*>(&width), reinterpret_cast<int*>(&height));

    // Setup HDR and MSAA Framebuffers
    setupHDRFramebuffer();
    setupMSAAFramebuffer();

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
    ImGui_ImplOpenGL3_Init("#version 330");

}

void SRender::render()

{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    OpenGlUtil::clearBackground();

    auto& registry = mScene->mEnttRegistry;

    const auto& view_matrix = mScene->mMainCamera.GetViewMatrix();
    const auto& proj_matrix = mScene->mMainCamera.GetProjectionMatrix();

    auto skybox = GameManager::mGraphicsManager->getCubemap("Skybox");
    auto shader = GameManager::mGraphicsManager->getShader("PBR");
    shader->use();
    shader->setMat4("projection", proj_matrix);
    shader->setMat4("view", view_matrix);
    shader->setVec3("viewPos", mScene->mMainCamera.Position);
	shader->setInt("environmentMap", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getID());
    
	buildDirectionalLights(*shader);
	buildPointLights(*shader);

	

    // Draw models
    drawModels(*shader);

    auto skyboxShader = GameManager::mGraphicsManager->getShader("Skybox");


    skyboxShader->use();
    glm::mat4 view_no_translation = glm::mat4(glm::mat3(view_matrix));
    skyboxShader->setMat4("view", view_no_translation);
    skyboxShader->setMat4("projection", proj_matrix);

    skybox->draw(*skyboxShader);

    drawImGui();
    
}

void SRender::shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void SRender::buildDirectionalLights(const Shader& shader)
{
    auto dirLightView = mScene->mEnttRegistry.view<CDirectionalLight>();
    int numDirLights = 0;
    for (auto entity : dirLightView)
    {
        auto& light = dirLightView.get<CDirectionalLight>(entity);
        std::string index = std::to_string(numDirLights);
        shader.setVec3("dirLights[" + index + "].direction", light.direction);
        shader.setVec3("dirLights[" + index + "].ambient", light.ambient);
        shader.setVec3("dirLights[" + index + "].diffuse", light.diffuse);
        shader.setVec3("dirLights[" + index + "].specular", light.specular);
        numDirLights++;
    }
    shader.setInt("numDirLights", numDirLights);
}

void SRender::buildPointLights(const Shader& shader)
{
    auto pointLightView = mScene->mEnttRegistry.view<CPointLight>();
    int numPointLights = 0;
    for (auto entity : pointLightView)
    {
        auto& light = pointLightView.get<CPointLight>(entity);
        std::string index = std::to_string(numPointLights);
        shader.setVec3("pointLights[" + index + "].position", light.position);
        shader.setVec3("pointLights[" + index + "].ambient", light.ambient);
        shader.setVec3("pointLights[" + index + "].diffuse", light.diffuse);
        shader.setVec3("pointLights[" + index + "].specular", light.specular);
        shader.setFloat("pointLights[" + index + "].constant", light.constant);
        shader.setFloat("pointLights[" + index + "].linear", light.linear);
        shader.setFloat("pointLights[" + index + "].quadratic", light.quadratic);
        numPointLights++;
    }
    shader.setInt("numPointLights", numPointLights);
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

        shader.setMat4("model", model_matrix);
        GameManager::mGraphicsManager->getModel(modelComp.name)->draw(shader);
    }
}


void SRender::drawImGui() const
{
    ImGui::Begin("Hello, ImGui!");
    ImGui::Text("This is a simple test window.");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SRender::setupHDRFramebuffer()
{
}

void SRender::setupMSAAFramebuffer()
{
}


