#include "PlayScene.hpp"
#include "GameManager.hpp"
#include "raylib.h"
int main()
{
    InitWindow(Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, "raylib [core] example - basic window");
    SetTargetFPS(Settings::MAX_FPS);
    const auto gameManager = GameManager::GetInstance();
    gameManager->LoadScene<PlayScene>();
    gameManager->Init();
    while (!WindowShouldClose())
    {
        gameManager->Update(GetFrameTime());
        //GameManager::GetInstance()->GetThreadPool()->enqueue([]() { GameManager::GetInstance()->Update(GetFrameTime()); });    //std::thread updateThread(&SceneManager::Update, SceneManager::GetInstance(), GetFrameTime());
        gameManager->Render();
    }
    gameManager->Shutdown();
    CloseWindow();
    return 0;
}
