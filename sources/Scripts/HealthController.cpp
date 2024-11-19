#include "HealthController.hpp"
#include "Scripts/Health.hpp"
void HealthController::Update(float deltaTime)
{
    auto ids = m_Registry->getEntityIDs<Health>();
    for( auto id : ids )
    {
		auto & health = m_Registry->getComponent<Health>(id);
		if( health.health <= 0 )
		{
		 m_Registry->deleteEntity(id);
		}
    }
}
