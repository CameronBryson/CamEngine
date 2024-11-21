#pragma once
#include "Engine/ScriptBase.hpp"
class WaveController : public ScriptBase
{
  public:
	explicit WaveController(Registry* registry, unsigned owner_ID) : ScriptBase(registry, owner_ID)
	{
	}
  private:
	void Update(float deltaTime) override;
	float time_since_enemy_spawn = 0.0f;
	float enemy_spawn_cooldown = 3.0f;

	float time_since_asteroid_spawn = 0.0f;
	float asteroid_spawn_cooldown = 5.0f;

	float time_since_debris_spawn = 0.0f;
	float debris_spawn_cooldown = 8.0f;

	glm::ivec2 spawn_range_x = { -30, 30 };
	glm::ivec2 spawn_range_y = { -15, 5 };

};