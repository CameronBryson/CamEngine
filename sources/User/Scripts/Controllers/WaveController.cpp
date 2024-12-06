#include "Engine/pch.hpp"
#include "WaveController.hpp"

#include "Engine/Util/GameSettings.hpp"
#include "Engine/Events/EventHandler.hpp"

void WaveController::update(float deltaTime)
{

    timeSinceEnemySpawn += deltaTime;
    timeSinceAsteroidSpawn += deltaTime;
    timeSinceDebrisSpawn += deltaTime;
    if( timeSinceEnemySpawn >= enemySpawnCooldown )
    {
	timeSinceEnemySpawn = 0.0f;
	int x = rand() % spawn_range_x.y * 2 + spawn_range_x.x;
	int y = rand() % spawn_range_y.y * 2 + spawn_range_y.x;
	glm::vec3 position = { x, y, -125 };
	EventHandler::GetInstance()->factoryDispatcher.SendEvent(CreateEnemyEvent(position, 3, glm::vec3{ 0, 0, 1 }, 15, settings::player_id));
    }
    if( timeSinceAsteroidSpawn >= asteroidSpawnCooldown )
    {
	timeSinceAsteroidSpawn = 0.0f;
	int x = rand() % spawn_range_x.y * 2 + spawn_range_x.x;
	int y = rand() % spawn_range_y.y * 2 + spawn_range_y.x;
	glm::vec3 position = { x, y, -190 };
	EventHandler::GetInstance()->factoryDispatcher.SendEvent(CreateAsteroidEvent(position, 4, 20, settings::player_id));
    }
    if( timeSinceDebrisSpawn >= asteroidSpawnCooldown )
    {
	timeSinceDebrisSpawn = 0.0f;
	int x = rand() % spawn_range_x.y * 2 + spawn_range_x.x;
	int y = rand() % spawn_range_y.y * 2 + spawn_range_y.x;
	glm::vec3 position = { x, y, -150 };
	EventHandler::GetInstance()->factoryDispatcher.SendEvent(CreateSpaceDebrisEvent(position, glm::vec3{ 0.1, 0.1, 1 }, glm::vec3{ 0.5, 0.5, 0.5 }, 10));
    }
}
