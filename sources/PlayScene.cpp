#include "PlayScene.hpp"

#include "CPlayer.hpp"
#include "CRender.hpp"
#include "CRigidbody.hpp"
#include "CTransform.hpp"
#include "Registry.hpp"
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

    m_Registry.pool->enqueue(&SRender::Init,&m_renderSystem);
    //m_renderSystem.Init();

    auto player = m_Registry.createEntity();
    m_Registry.addComponent(player, CTransform({100,100}));
    m_Registry.addComponent(player,CPlayer());
    m_Registry.addComponent(player, CRigidbody(0,0.7));



}

void PlayScene::Update(float dt)
{

    //m_Registry->pool->enqueue(&SPlayer::Update, *m_playerSystem, *m_Registry);
    //registry pointer that is being based in is being destroyed and causing the deconstrucot to destory the pool
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
    m_Registry.createSparseSet<CRigidbody>();
    m_Registry.createSparseSet<CPlayer>();
}
