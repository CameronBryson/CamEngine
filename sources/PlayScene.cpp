#include "PlayScene.hpp"

#include <GameManager.hpp>

#include "Components.hpp"
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
    InitSparseSets();

    m_renderSystem.Init();

    auto player = m_Registry.createEntity();

    m_Registry.addComponent<CTransform>(player,CTransform());
    m_Registry.addComponent<CRigidBody>(player,CRigidBody(0,0.7));
    m_Registry.addComponent<CPlayer>(player, CPlayer());
    auto player2 = m_Registry.createEntity();
    m_Registry.addComponent<CTransform>(player2,CTransform());
    m_Registry.addComponent<CRigidBody>(player2,CRigidBody(0,0.7));
    m_Registry.addComponent<CPlayer>(player2, CPlayer());



}

void PlayScene::Update(float dt)
{
    Timer updateTimer(Stats::StatType::UPDATE);

    // Enqueue separate tasks for player and physics system updates
    auto& threadPool = *GameManager::GetInstance()->GetThreadPool();
    std::future<void> playerUpdate = threadPool.enqueue([this, dt]() {
        m_playerSystem.Update(m_Registry, dt);
        m_physicsSystem.Update(m_Registry, dt);
    });
    // std::future<void> physicsUpdate = threadPool.enqueue([this, dt]() {
    //     m_physicsSystem.Update(m_Registry, dt);
    // });

    // Wait for both updates to complete before processing commands
    playerUpdate.wait();
    //physicsUpdate.wait();

    m_Registry.processCommands();
}

void PlayScene::Render()
{
    m_renderSystem.Update(m_Registry);
}

void PlayScene::Shutdown()
{
    m_renderSystem.Shutdown();
    m_playerSystem.Shutdown();
    m_physicsSystem.Shutdown();
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
}
