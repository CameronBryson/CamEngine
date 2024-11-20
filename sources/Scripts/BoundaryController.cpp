#include "BoundaryController.hpp"

void BoundaryController::Update(float deltaTime)
{
	auto & transforms = m_Registry->getSparseSet<CTransform>();
	auto ids = m_Registry->getEntityIDs<CTransform>();
	for( auto id : ids )
	{
		auto & transform = m_Registry->getComponent<CTransform>(id);
		if( transform.position.x < settings::world_boundry_min.x )
		{
			m_Registry->deleteEntity(id);
		}
		if( transform.position.y < settings::world_boundry_min.y )
		{
			m_Registry->deleteEntity(id);
		}
		if( transform.position.z < settings::world_boundry_min.z )
		{
			m_Registry->deleteEntity(id);
		}
		if( transform.position.x > settings::world_boundry_max.x )
		{
			m_Registry->deleteEntity(id);
		}
		if( transform.position.y > settings::world_boundry_max.y )
		{
			m_Registry->deleteEntity(id);
		}
		if( transform.position.z > settings::world_boundry_max.z )
		{
			m_Registry->deleteEntity(id);
		}
	}
}
