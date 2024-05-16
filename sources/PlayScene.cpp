#include "PlayScene.hpp"

#include "CPlayer.hpp"
#include "CRender.hpp"
#include "CRigidbody.hpp"
#include "CTransform.hpp"
#include "Registry.hpp"
PlayScene::PlayScene() {
    m_Registry = new Registry();
    m_playerSystem = new SPlayer();
    m_physicsSystem = new SPhysics();
    m_renderSystem = new SRender();
}
PlayScene::~PlayScene() {
    delete m_Registry;
    delete m_playerSystem;
    delete m_physicsSystem;
    delete m_renderSystem;
}
void PlayScene::Init()
{
    InitSparseSets();

    m_Registry->pool->enqueue(&SRender::Init,*m_renderSystem);
    m_renderSystem->Init();

    auto player = m_Registry->createEntity();
    m_Registry->addComponent(player, CTransform({100,100}));
    m_Registry->addComponent(player,CPlayer());
    m_Registry->addComponent(player, CRigidbody(0,0.7));



}

void PlayScene::Update(float dt)
{

    //m_Registry->pool->enqueue(&SPlayer::Update, *m_playerSystem, *m_Registry);
    //registry pointer that is being based in is being destroyed and causing the deconstrucot to destory the pool
    m_playerSystem->Update(*m_Registry);
    m_physicsSystem->Update(*m_Registry);
}

void PlayScene::Render()
{
    m_renderSystem->Update(*m_Registry);
}

void PlayScene::Shutdown()
{
    m_renderSystem->Shutdown();
    m_playerSystem->Shutdown();
    m_physicsSystem->Shutdown();
}
Registry* PlayScene::GetRegistry() {
    return m_Registry;
}
void PlayScene::InitSparseSets() {
    m_Registry->createSparseSet<CTransform>();
    m_Registry->createSparseSet<CRender>();
    m_Registry->createSparseSet<CRigidbody>();
    m_Registry->createSparseSet<CPlayer>();
}
