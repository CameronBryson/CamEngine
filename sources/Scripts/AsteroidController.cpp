#include "AsteroidController.hpp"
#include "Scripts/Asteroid.hpp"
void AsteroidController::Update(float deltaTime)
{
	auto& asteroids = m_Scene->getSparseSet<Asteroid>();
	auto& transforms = m_Scene->getSparseSet<CTransform>();
	auto& dynamic_bodies = m_Scene->getSparseSet<CDynamicBody>();
	auto ids = m_Scene->getEntityIDs<Asteroid, CTransform, CDynamicBody>();
	for( auto id : ids )
	{
		auto& asteroid = asteroids.get_item(id);
		auto& transform = transforms.get_item(id);
		auto& dynamic_body = dynamic_bodies.get_item(id);
		auto target_position = transforms.get_item(asteroid.target).position;

		auto direction = glm::normalize(target_position - transform.position) + glm::vec3{ 0, 0, 1 };
	//dynamic_body.acceleration+=direction*asteroid.speed;
	//dynamic_body.acceleration+=glm::vec3{0,0,-1} * asteroid.speed;
	}
}
