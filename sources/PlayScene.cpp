#include "PlayScene.hpp"

#include "CBoxBounds.hpp"
#include "CCircleBounds.hpp"
#include "CRender.hpp"
#include "CRigidbody.hpp"
#include "CTransform.hpp"
#include "Registry.hpp"

void PlayScene::Init()
{
    InitSparseSets();
    m_renderSystem.Init();
}

void PlayScene::Update(float dt)
{

}

void PlayScene::Render()
{
    m_renderSystem.Update();
}

void PlayScene::Shutdown()
{
    m_renderSystem.Shutdown();
}
Registry &PlayScene::GetRegistry() {
    return m_Registry;
}
void PlayScene::InitSparseSets() {
    m_Registry.createSparseSet<CTransform>();
    m_Registry.createSparseSet<CRender>();
    m_Registry.createSparseSet<CRigidbody>();
    m_Registry.createSparseSet<CBoxBounds>();
    m_Registry.createSparseSet<CCircleBounds>();
}
