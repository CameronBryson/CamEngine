#include "GameManager.hpp"
#include "PlayScene.hpp"
#include "raylib.h"
int main()
{
    InitWindow(settings::window_width, settings::window_height, "raylib [core] example - basic window");
    SetTargetFPS(settings::max_fps);
    const auto game_manager = game_manager::get_instance();
    game_manager->load_scene<play_scene>();
    game_manager->init();
    while (!WindowShouldClose())
    {
        game_manager->update(GetFrameTime());
        game_manager->render();
    }
    game_manager->shutdown();
    CloseWindow();
    return 0;
}
