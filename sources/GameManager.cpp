#include "GameManager.hpp"
#include <memory>
GameManager* GameManager::m_instance = nullptr;

GameManager::GameManager()
{
    m_currentScene = nullptr;
    m_threadPool = new ThreadPool();
}

GameManager::~GameManager()
{
    Shutdown();
}

GameManager* GameManager::GetInstance()
{
    if (m_instance == nullptr){
        m_instance = new GameManager();
    }
    return m_instance;
}

void GameManager::Init() const {
    m_currentScene->Init();
}

void GameManager::Update(float dt)
{
    m_currentScene->Update(dt);
}

void GameManager::Render()
{
    m_currentScene->Render();
}

void GameManager::Shutdown() {
    m_currentScene->Shutdown();
    delete m_currentScene;
    delete m_threadPool;
    // delete m_instance;
}
ThreadPool &GameManager::GetThreadPool() const {
    return *m_threadPool;
}
