#include "PlayScene.hpp"
#include "SceneManager.hpp"
#include "raylib.h"
#include "Delegate.hpp"
int main()
{

    SceneManager::GetInstance()->LoadScene<PlayScene>();
    SceneManager::GetInstance()->Init();

    while (!WindowShouldClose())
    {
        SceneManager::GetInstance()->Update(0.0f);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }
    SceneManager::GetInstance()->Shutdown();
    return 0;
}
