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
    InitSparseSets();

    SRender::Init();
    Timer benchmarkTimer(Stats::StatType::BENCHMARK);
    for (int i = 1; i < Settings::MAX_ENTITIES-1; i++) {

        auto entity = m_Registry.createEntity();
        m_Registry.addComponent<CPosition>(entity, CPosition());
        m_Registry.addComponent<CRigidBody>(entity, CRigidBody());
        m_Registry.addComponent<CVelocity>(entity, CVelocity());
        m_Registry.addComponent<CPlayer>(entity, CPlayer());
    }

    // for (int i = 1; i < Settings::MAX_ENTITIES-1; i++) {
    //     m_Registry.deleteEntity(i);
    // }
    Factory::CreatePlayer(m_Registry);
//if there arent any spots left the new component wont have the new rigidbody properties

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

    m_Registry.ProcessCommands();

}

void PlayScene::Render()
{
    Timer renderTimer(Stats::StatType::RENDER);
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
