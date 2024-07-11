#include "PlayScene.hpp"
#include "SceneManager.hpp"
#include "raylib.h"
static double UpdateStart = 0;
static double UpdateEnd = 0;
int main()
{
    InitWindow(Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, "raylib [core] example - basic window");
    SetTargetFPS(Settings::MAX_FPS);

    SceneManager::GetInstance()->LoadScene<PlayScene>();
    SceneManager::GetInstance()->Init();

    while (!WindowShouldClose())
    {
        SceneManager::GetInstance()->Update(GetFrameTime());
        SceneManager::GetInstance()->Render();
    }
    SceneManager::GetInstance()->Shutdown();
    CloseWindow();
    return 0;
}
