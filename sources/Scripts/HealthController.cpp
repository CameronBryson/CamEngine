#include "HealthController.hpp"
#include "Scripts/Health.hpp"
void HealthController::Update(float deltaTime)
{
	auto ids = m_Scene->getEntityIDs<Health>();
	for( auto id : ids )
	{
		auto & health = m_Scene->getComponent<Health>(id);
		if( health.health <= 0 )
		{
		 m_Scene->deleteEntity(id);
		}
	}
}
