#include "Engine/pch.hpp"
#include "HealthController.hpp"
#include "User/Scripts/Components/Health.hpp"
#include "User/Scripts/Components/Damage.hpp"
#include "Engine/Base/BaseScene.hpp"

void HealthController::onCollisionEnter(unsigned short firstID, unsigned short secondID)
{
	if (GetScene().hasComponent<Health>(firstID) && GetScene().hasComponent<Damage>(secondID))
	{
		auto& health = GetScene().getComponent<Health>(firstID);
		auto& damage = GetScene().getComponent<Damage>(secondID);
		health.health -= damage.damage;
		printf("Health: %f\n", health.health);
	}

	if (GetScene().hasComponent<Health>(secondID) && GetScene().hasComponent<Damage>(firstID))
	{
		auto& health = GetScene().getComponent<Health>(secondID);
		auto& damage = GetScene().getComponent<Damage>(firstID);
		health.health -= damage.damage;
		printf("Health: %f\n", health.health);
	}
}

void HealthController::update(float deltaTime)
{
	const auto ids = GetScene().getEntityIDs<Health>();
	std::vector<unsigned short> entitiesToDelete;

	for (auto id : ids)
	{
		auto& health = GetScene().getComponent<Health>(id);
		if (health.health <= 0)
		{
			entitiesToDelete.push_back(id);
		}
	}

	for (auto id : entitiesToDelete)
	{
		GetScene().deleteEntity(id);
	}
}
