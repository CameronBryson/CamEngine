#include "PlayScene.hpp"

#include "CBoxBounds.hpp"
#include "Registry.hpp"

void PlayScene::Init()
{
    m_Registry.createSparseSet<CBoxBounds>();

}

void PlayScene::Update(float dt)
{
}

void PlayScene::Render()
{
}

void PlayScene::Shutdown()
{
}
Registry& PlayScene::GetRegistry(){
  return m_Registry;
}
