#pragma once
#include <glm/vec3.hpp>

namespace settings
{
    #define LAYER_PLAYER  0x00000001  // 1st bit for player layer
    #define LAYER_ENEMY   0x00000002  // 2nd bit for enemy layer
    inline unsigned int player_bitmask = LAYER_ENEMY; // Player collides with enemies, power-ups, and terrain
    inline unsigned int enemy_bitmask   = LAYER_PLAYER;           // Enemy collides with players

    inline int window_width = 1920;
    inline int window_height = 1080;
    inline float aspect_ratio = (float) window_width / (float) window_height;
    inline glm::vec3 world_boundry_min = {-30,-30,-50};
    inline glm::vec3 world_boundry_max = {30,30,5};
    constexpr unsigned max_fps = 600;
    constexpr unsigned max_entities = 10000;

} // namespace Settings
