#include "AsteroidController.hpp"
#include "User/Scripts/Components/Asteroid.hpp"
#include "Engine/Base/BaseScene.hpp"
void AsteroidController::update(float deltaTime)
{
	auto& asteroids = GetScene().getSparseSet<Asteroid>();
	auto& transforms = GetScene().getSparseSet<CTransform>();
	auto& dynamic_bodies = GetScene().getSparseSet<CDynamicBody>();
	auto ids = GetScene().getEntityIDs<Asteroid, CTransform, CDynamicBody>();
	for( auto id : ids )
	{
		auto& asteroid = asteroids.get_item(id);
		auto& transform = transforms.get_item(id);
		auto& dynamicBody = dynamic_bodies.get_item(id);
		auto target_position = transforms.get_item(asteroid.target).position;

		auto direction = glm::normalize(target_position - transform.position) + glm::vec3{ 0, 0, 1 };
	//dynamic_body.acceleration+=direction*asteroid.speed;
	//dynamic_body.acceleration+=glm::vec3{0,0,-1} * asteroid.speed;
	}
}
