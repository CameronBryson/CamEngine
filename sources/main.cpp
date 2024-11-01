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
    game_manager::load_scene<play_scene>();
    //game_manager::init();
    //game_manager::load_scene<play_scene>();
    game_manager::init();
    game_manager::game_loop();
    game_manager::shutdown();

    return 0;
}