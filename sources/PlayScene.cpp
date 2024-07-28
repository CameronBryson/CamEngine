#include "PlayScene.hpp"

#include <GameManager.hpp>

#include "Components.hpp"
#include "Factory.hpp"
#include "Registry.hpp"
#include "Timer.hpp"
#include "raylib.h"
PlayScene::PlayScene() {
    printf("PlayScene created\n");

}
PlayScene::~PlayScene() {
    printf("PlayScene destroyed\n");


}
void PlayScene::Init()
{
    m_Camera.position = {0.0f, 10.0f, 10.0f};
    m_Camera.target = {0.0f, 0.0f, 0.0f};
    m_Camera.up = {0.0f, 1.0f, 0.0f};
    m_Camera.fovy = 45.0f;
    m_Camera.projection = CAMERA_PERSPECTIVE;
    InitSparseSets();

    SRender::Init();
    Timer benchmarkTimer(Stats::StatType::BENCHMARK);
    for (int i = 1; i < Settings::MAX_ENTITIES-1; i++) {

        auto entity = m_Registry.createEntity();
        m_Registry.addComponent<CTransform>(entity, CTransform());
        m_Registry.addComponent<CRigidBody>(entity, CRigidBody());
        m_Registry.addComponent<CVelocity>(entity, CVelocity());
        m_Registry.addComponent<CPlayer>(entity, CPlayer());
    }
    for (int i = 1; i < Settings::MAX_ENTITIES-1; i++) {
        m_Registry.deleteEntity(i);
    }
    Factory::CreatePlayer(m_Registry);
}

void PlayScene::Update(float dt)
{
    Timer updateTimer(Stats::StatType::UPDATE);
    SPlayer::Update(m_Registry, dt);
    SPhysics::Update(m_Registry, dt);
    m_Registry.ProcessCommands();

}

void PlayScene::Render()
{
    Timer renderTimer(Stats::StatType::RENDER);
    SRender::Update(m_Registry, m_Camera);
}

void PlayScene::Shutdown()
{
    SRender::Shutdown();
    SPlayer::Shutdown();
    SPhysics::Shutdown();
}
Registry& PlayScene::GetRegistry() {
    return m_Registry;
}
void PlayScene::InitSparseSets() {
    m_Registry.createSparseSet<CTransform>();
    m_Registry.createSparseSet<CRender>();
    m_Registry.createSparseSet<CRigidBody>();
    m_Registry.createSparseSet<CPlayer>();
    m_Registry.createSparseSet<CSprite>();
    m_Registry.createSparseSet<CStaticBody>();
    m_Registry.createSparseSet<CKineticBody>();
    m_Registry.createSparseSet<CVelocity>();
}
