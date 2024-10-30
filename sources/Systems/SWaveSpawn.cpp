//
// Created by cam on 29/10/24.
//

#include "SWaveSpawn.hpp"

#include <GameSettings.hpp>
#include <Engine/EventHandler.hpp>

void s_wave_spawn::update(registry & registry, float dt)
{
    time_since_enemy_spawn+=dt;
    time_since_asteroid_spawn+=dt;
    time_since_debris_spawn+=dt;
    if(time_since_enemy_spawn>=enemy_spawn_cooldown)
    {
        time_since_enemy_spawn = 0.0f;
        int x = rand() % spawn_range_x.y*2 + spawn_range_x.x;
        int y = rand() % spawn_range_y.y*2 + spawn_range_y.x;
        glm::vec3 position = {x,y,-125};
        EventHandler::GetInstance()->factory_dispatcher.SendEvent(CreateEnemyEvent(position,3,glm::vec3{0,0,1},15,settings::player_id));
    }
    if(time_since_asteroid_spawn>=asteroid_spawn_cooldown)
    {
        time_since_asteroid_spawn = 0.0f;
        int x = rand() % spawn_range_x.y*2 + spawn_range_x.x;
        int y = rand() % spawn_range_y.y*2 + spawn_range_y.x;
        glm::vec3 position = {x,y,-190};
        EventHandler::GetInstance()->factory_dispatcher.SendEvent(CreateAsteroidEvent(position,4,20,settings::player_id ));
    }
    if(time_since_debris_spawn>=asteroid_spawn_cooldown)
    {
        time_since_debris_spawn = 0.0f;
        int x = rand() % spawn_range_x.y*2 + spawn_range_x.x;
        int y = rand() % spawn_range_y.y*2 + spawn_range_y.x;
        glm::vec3 position = {x,y,-150};
        EventHandler::GetInstance()->factory_dispatcher.SendEvent(CreateSpaceDebrisEvent(position,glm::vec3{0.1,0.1,1},glm::vec3{0.5,0.5,0.5},10));
    }
}

