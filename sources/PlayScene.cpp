#include "PlayScene.hpp"

#include <GameManager.hpp>

#include "Components.hpp"
#include "Registry.hpp"
#include "Timer.hpp"
#include "raylib.h"
PlayScene::PlayScene() : m_threadPool(GameManager::GetInstance()->GetThreadPool()) {
    printf("PlayScene created\n");

}
PlayScene::~PlayScene() {
    printf("PlayScene destroyed\n");


}
void PlayScene::Init()
{
    InitSparseSets();

    SRender::Init();

    auto player = m_Registry.createEntity();

    m_Registry.addComponent<CPosition>(player,CPosition());
    m_Registry.addComponent<CRigidBody>(player,CRigidBody{.mass = 0, .drag = 0.7});
    m_Registry.addComponent<CPlayer>(player, CPlayer());
    m_Registry.addComponent<CVelocity>(player, CVelocity());
    auto player2 = m_Registry.createEntity();
    m_Registry.addComponent<CPosition>(player2,CPosition());
    m_Registry.addComponent<CRigidBody>(player2,CRigidBody{.mass = 0, .drag = 0.7});
    m_Registry.addComponent<CPlayer>(player2, CPlayer());
    m_Registry.addComponent<CVelocity>(player2, CVelocity());



}

void PlayScene::Update(float dt)
{
    Timer updateTimer(Stats::StatType::UPDATE);

    // Enqueue separate tasks for player and physics system updates
    // std::future<void> playerUpdate = m_threadPool.enqueue([this, dt] {
    //     SPlayer::Update(m_Registry, dt);
    //     SPhysics::Update(m_Registry, dt);
    // });
    // playerUpdate.wait();
    SPlayer::Update(m_Registry, dt);
    SPhysics::Update(m_Registry, dt);
}

void PlayScene::Render()
{
    SRender::Update(m_Registry);
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
    m_Registry.createSparseSet<CPosition>();
    m_Registry.createSparseSet<CRender>();
    m_Registry.createSparseSet<CRigidBody>();
    m_Registry.createSparseSet<CPlayer>();
    m_Registry.createSparseSet<CSprite>();
    m_Registry.createSparseSet<CStaticBody>();
    m_Registry.createSparseSet<CKineticBody>();
    m_Registry.createSparseSet<CVelocity>();
}
