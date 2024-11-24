#include "HealthController.hpp"
#include "Scripts/Health.hpp"
#include "Engine/BaseScene.hpp"
void HealthController::update(float deltaTime)
{
	auto ids = GetScene().getEntityIDs<Health>();
	for( auto id : ids )
	{
		auto & health = GetScene().getComponent<Health>(id);
		if( health.health <= 0 )
		{
		 GetScene().deleteEntity(id);
		}
	}
}
