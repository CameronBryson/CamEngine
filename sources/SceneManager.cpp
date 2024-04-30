#include "SceneManager.hpp"
#include <memory>
SceneManager* SceneManager::m_instance = nullptr;

SceneManager::SceneManager()
{
    m_currentScene = nullptr;
}

SceneManager::~SceneManager()
{
    Shutdown();
}

SceneManager* SceneManager::GetInstance()
{
    if (m_instance == nullptr){
        m_instance = new SceneManager();
    }
    return m_instance;
}

void SceneManager::Init()
{
    m_currentScene->Init();
}

void SceneManager::Update(float dt)
{
    m_currentScene->Update(dt);
}

void SceneManager::Render()
{
    m_currentScene->Render();
}

void SceneManager::Shutdown()
{
    m_currentScene->Shutdown();
}
