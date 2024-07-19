#include "PlayScene.hpp"
#include "GameManager.hpp"
#include "raylib.h"
int main()
{
    InitWindow(Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, "raylib [core] example - basic window");
    SetTargetFPS(Settings::MAX_FPS);
    GameManager::GetInstance()->LoadScene<PlayScene>();
    GameManager::GetInstance()->Init();
    while (!WindowShouldClose())
    {
        //GameManager::GetInstance()->Update(GetFrameTime());
        GameManager::GetInstance()->GetThreadPool()->enqueue([]() { GameManager::GetInstance()->Update(GetFrameTime()); });    //std::thread updateThread(&SceneManager::Update, SceneManager::GetInstance(), GetFrameTime());
        GameManager::GetInstance()->Render();
    }
    GameManager::GetInstance()->Shutdown();
    CloseWindow();
    return 0;
}
