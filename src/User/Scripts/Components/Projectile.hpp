#pragma once
#include "glm/vec3.hpp"
struct Projectile
{
	Projectile(const glm::vec3& direction, float speed) : direction(direction), speed(speed) {}
	glm::vec3 direction;
	float speed;
};