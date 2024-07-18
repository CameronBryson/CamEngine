#include "PlayScene.hpp"

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
    m_Registry.addComponent(player, CTransform({100,100}));
    m_Registry.addComponent(player,CPlayer());
    m_Registry.addComponent(player, CRigidBody(0,0.7));



}

void PlayScene::Update(float dt)
{
    Timer updateTimer(Stats::StatType::UPDATE);
    m_playerSystem.Update(&m_Registry,dt);
    m_physicsSystem.Update(&m_Registry,dt);

}

void PlayScene::Render()
{
    m_renderSystem.Update(&m_Registry);
}

void PlayScene::Shutdown()
{
    m_renderSystem.Shutdown();
    m_playerSystem.Shutdown();
    m_physicsSystem.Shutdown();
}
Registry* PlayScene::GetRegistry() {
    return &m_Registry;
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
