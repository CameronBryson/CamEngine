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

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include <entt/entity/utility.hpp>
#include <GameSettings.hpp>

SRender::SRender(BaseScene* scene) : mScene(scene)
{
}

void SRender::init()
{
    glClearColor(0, 0, 0, 0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Enable backface culling
    glEnable(GL_CULL_FACE);

    // Specify that we want to cull back faces
    glCullFace(GL_BACK);

    // Optionally, specify the front face winding order (default is GL_CCW)
    glFrontFace(GL_CCW);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glDepthMask(GL_FALSE);
}

void SRender::render()
{
    OpenGlUtil::clearBackground();

    auto& registry = mScene->mEnttRegistry;

    const auto& view_matrix = mScene->mMainCamera.GetViewMatrix();
    const auto& proj_matrix = mScene->mMainCamera.GetProjectionMatrix();

    auto texture_shader_3D = GameManager::mGraphicsManager->getShader("PBR");
    // auto color_shader_3D = GameManager::mGraphicsManager->getShader("3D_color");
    // auto color_shader_2D = GameManager::mGraphicsManager->getShader("2D_color");
    // auto texture_shader_2D = GameManager::mGraphicsManager->getShader("2D_texture");

    // Set up orthographic projection
    int windowWidth = settings::window_width;
    int windowHeight = settings::window_height;
    glm::mat4 ortho_projection = glm::ortho(
        0.0f, static_cast<float>(windowWidth),
        0.0f, static_cast<float>(windowHeight)
    );

    glEnable(GL_DEPTH_TEST);

    texture_shader_3D->use();
    texture_shader_3D->setMat4("projection", proj_matrix);
    texture_shader_3D->setMat4("view", view_matrix);
    texture_shader_3D->setVec3("viewPos", mScene->mMainCamera.Position);

    // Setup directional lights
    auto dirLightView = registry.view<CDirectionalLight>();
    int numDirLights = 0;
    for (auto entity : dirLightView)
    {
        auto& light = dirLightView.get<CDirectionalLight>(entity);
        std::string index = std::to_string(numDirLights);
        texture_shader_3D->setVec3("dirLights[" + index + "].direction", light.direction);
        texture_shader_3D->setVec3("dirLights[" + index + "].ambient", light.ambient);
        texture_shader_3D->setVec3("dirLights[" + index + "].diffuse", light.diffuse);
        texture_shader_3D->setVec3("dirLights[" + index + "].specular", light.specular);
        numDirLights++;
    }
    texture_shader_3D->setInt("numDirLights", numDirLights);

    // Setup point lights
    auto pointLightView = registry.view<CPointLight, CTransform>();
    int numPointLights = 0;
    for (auto entity : pointLightView)
    {
        auto& light = pointLightView.get<CPointLight>(entity);
        auto& transform = pointLightView.get<CTransform>(entity);
        std::string index = std::to_string(numPointLights);
        texture_shader_3D->setVec3("pointLights[" + index + "].position", transform.position);
        texture_shader_3D->setVec3("pointLights[" + index + "].ambient", light.ambient);
        texture_shader_3D->setVec3("pointLights[" + index + "].diffuse", light.diffuse);
        texture_shader_3D->setVec3("pointLights[" + index + "].specular", light.specular);
        texture_shader_3D->setFloat("pointLights[" + index + "].constant", light.constant);
        texture_shader_3D->setFloat("pointLights[" + index + "].linear", light.linear);
        texture_shader_3D->setFloat("pointLights[" + index + "].quadratic", light.quadratic);
        numPointLights++;
    }
    texture_shader_3D->setInt("numPointLights", numPointLights);

    // Draw models
    drawModels(*texture_shader_3D);

    // Uncomment and update the following code if needed
    // #ifdef _DEBUG
    // color_shader_3D->use();
    // color_shader_3D->setMat4("projection", proj_matrix);
    // color_shader_3D->setMat4("view", view_matrix);
    // drawColliders(*color_shader_3D);
    // #endif

    // Draw UI elements
    // color_shader_2D->use();
    // color_shader_2D->setMat4("projection", ortho_projection);
    // color_shader_2D->setMat4("view", glm::mat4(1.0f));
    // drawUi(*color_shader_2D);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // glDisable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Render text
    /*auto textShader = GameManager::mGraphicsManager->getShader("text");
    textShader->use();
    textShader->setMat4("projection", ortho_projection);

    auto textView = registry.view<CText>();
    auto font = GameManager::mGraphicsManager->getFont("arial");

    for (auto entity : textView)
    {
        auto& text = textView.get<CText>(entity);
        font->renderText(*textShader, text.text, text.position.x,
            text.position.y, text.font_size, text.color);
    }*/

    // glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void SRender::shutdown()
{
}

void SRender::drawModels(const Shader& shader) const
{
    auto& registry = mScene->mEnttRegistry;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Draw background models first
    auto backgroundModelView = registry.view<CModel, CTransform, CBackground>();
    for (auto entity : backgroundModelView)
    {
        auto& model = backgroundModelView.get<CModel>(entity);
        auto& transform = backgroundModelView.get<CTransform>(entity);

        glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
            glm::scale(glm::mat4(1.0f), transform.scale) *
            glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);

        shader.setMat4("model", model_matrix);
        GameManager::mGraphicsManager->getModel(model.name)->draw(shader, *GameManager::mGraphicsManager);
    }

    // Draw other models
    auto modelView = registry.view<CModel, CTransform>();
    for (auto entity : modelView)
    {
        auto& model = modelView.get<CModel>(entity);
        auto& transform = modelView.get<CTransform>(entity);

        glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
            glm::scale(glm::mat4(1.0f), transform.scale) *
            glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);

        shader.setMat4("model", model_matrix);
        GameManager::mGraphicsManager->getModel(model.name)->draw(shader, *GameManager::mGraphicsManager);
    }
}

void SRender::drawUi(const Shader& shader) const
{
    auto& registry = mScene->mEnttRegistry;

    glDisable(GL_DEPTH_TEST);

    auto uiView = registry.view<CUI, CTransform>();
    for (auto entity : uiView)
    {
        auto& transform = uiView.get<CTransform>(entity);

        glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), transform.position) *
            glm::scale(glm::mat4(1.0f), transform.scale) *
            glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);

        shader.setMat4("model", model_matrix);

        auto mesh = GameManager::mGraphicsManager->getMesh("Quad");
        mesh->draw(shader, *GameManager::mGraphicsManager);
    }
}
