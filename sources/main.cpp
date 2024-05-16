#include "PlayScene.hpp"
#include "SceneManager.hpp"
#include "raylib.h"
int main()
{
    InitWindow(Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, "raylib [core] example - basic window");
    SetTargetFPS(60);

    SceneManager::GetInstance()->LoadScene<PlayScene>();
    SceneManager::GetInstance()->Init();

    while (!WindowShouldClose())
    {
        SceneManager::GetInstance()->Update(0.0f);
        SceneManager::GetInstance()->Render();
    }
    SceneManager::GetInstance()->Shutdown();
    CloseWindow();
    return 0;
}
