#pragma once
#include "glm/vec3.hpp"
struct EnemyShip
{
	EnemyShip(unsigned short target, float speed, glm::vec3 direction) : target(target), speed(speed), direction(direction)
	{
	}
	unsigned short target;
	float speed = 1.0f;
	glm::vec3 direction = { 0, 0, 0 };
	float shootCooldown = 2.0f;
	float timeSinceShoot = 0.0f;
};