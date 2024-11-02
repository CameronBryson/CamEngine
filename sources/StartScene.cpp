//
// Created by cam on 29/10/24.
//

#include "StartScene.hpp"

#include <Systems/SRender.hpp>
#include <Graphics/Camera.hpp>
#include <Graphics/GraphicsManager.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Model.hpp>
#include <Graphics/Material.hpp>
#include <Graphics/Mesh.hpp>

StartScene::StartScene()
{
    printf("Start Scene created\n");
    m_camera_ = std::make_unique<Camera>(glm::vec3 {0.0f, 0.0f, 0.0f});
    m_registry_ = std::make_unique<Registry>();
    m_factory_ = std::make_unique<Factory>(getRegistry());
    m_system_render_ = std::make_unique<SRender>();
    m_graphics_manager_ = std::make_unique<GraphicsManager>();
}

StartScene::~StartScene()
{
    printf("Start Scene destroyed\n");
}

void StartScene::init()
{
    initSparseSets();
    m_system_render_->init(*m_graphics_manager_);
    m_factory_->createBoundary(*m_registry_, glm::vec3{-20,0,0}, glm::vec3{1,10,1});
}

void StartScene::update(float dt)
{
}

void StartScene::lateUpdate(float dt)
{
}

void StartScene::render()
{
    m_system_render_->update(getRegistry(),*m_graphics_manager_, *m_camera_);
}

void StartScene::shutdown()
{
}

Registry & StartScene::getRegistry()
{
    return *m_registry_;
}

void StartScene::initSparseSets()
{
    m_registry_->createSparseSet<CTransform>();
    m_registry_->createSparseSet<CModel>();
    m_registry_->createSparseSet<CUI>();
    m_registry_->createSparseSet<CDirectionalLight>();
    m_registry_->createSparseSet<CPointLight>();
    m_registry_->createSparseSet<CBackground>();
    m_registry_->createSparseSet<CQuad>();
    m_registry_->createSparseSet<CSphere>();
    m_registry_->createSparseSet<CCollider>();

}