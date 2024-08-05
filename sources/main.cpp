#include "Engine/GameManager.hpp"
#include "PlayScene.hpp"
int main()
{
    //InitWindow(settings::window_width, settings::window_height, "raylib [core] example - basic window");
    //SetTargetFPS(settings::max_fps);
    const auto game_manager = game_manager::get_instance();
    game_manager->load_scene<play_scene>();
    game_manager->init();
    game_manager->game_loop();
    game_manager->shutdown();
    return 0;
}
