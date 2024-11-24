#pragma once
#include "Engine/ScriptBase.hpp"
#include "glm/vec2.hpp"

class WaveController : public ScriptBase
{
  public:
	explicit WaveController(BaseScene* scene, unsigned short owner_ID) : ScriptBase(scene, owner_ID)
	{
	}
  private:
	void update(float deltaTime) override;
	float timeSinceEnemySpawn = 0.0f;
	float enemySpawnCooldown = 3.0f;

	float timeSinceAsteroidSpawn = 0.0f;
	float asteroidSpawnCooldown = 5.0f;

	float timeSinceDebrisSpawn = 0.0f;
	float debrisSpawnCooldown = 8.0f;

	glm::ivec2 spawn_range_x = { -30, 30 };
	glm::ivec2 spawn_range_y = { -15, 5 };

};
