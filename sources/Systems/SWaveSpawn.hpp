#pragma once
#include <glm/vec2.hpp>
class registry;
class s_wave_spawn{
public:
    void update(registry& registry, float dt);
private:
    float time_since_enemy_spawn = 0.0f;
    float enemy_spawn_cooldown = 5.0f;

    float time_since_asteroid_spawn = 0.0f;
    float asteroid_spawn_cooldown = 10.0f;

    float time_since_debris_spawn = 0.0f;
    float debris_spawn_cooldown = 12.0f;

    glm::ivec2 spawn_range_x = {-15,15};
    glm::ivec2 spawn_range_y = {-5,5};

};