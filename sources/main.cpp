#include "Engine/GameManager.hpp"
#include "PlayScene.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "StartScene.hpp"
#include "stb_image.h"

int main()
{
    //InitWindow(settings::window_width, settings::window_height, "raylib [core] example - basic window");
    //SetTargetFPS(settings::max_fps);
    //game_manager::load_scene<start_scene>();
    GameManager::loadScene<PlayScene>();
    //game_manager::init();
    //game_manager::load_scene<play_scene>();
    GameManager::init();
    GameManager::gameLoop();
    GameManager::shutdown();

    return 0;
}