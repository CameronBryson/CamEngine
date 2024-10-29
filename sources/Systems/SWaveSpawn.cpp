//
// Created by cam on 29/10/24.
//

#include "SWaveSpawn.hpp"

void s_wave_spawn::update(registry & registry, float dt)
{
    time_since_spawn+=dt;
    if(time_since_spawn>=spawn_cooldown)
    {
        //spawn
        time_since_spawn = 0.0f;
    }
}

