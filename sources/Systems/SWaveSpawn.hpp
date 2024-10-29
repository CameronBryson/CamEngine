#pragma once
class registry;
class s_wave_spawn{
public:
    void update(registry& registry, float dt);
private:
    float time_since_spawn = 0.0f;
    float spawn_cooldown = 3.0f;

};