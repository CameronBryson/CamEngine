#include "BoundaryController.hpp"

void BoundaryController::Update(float deltaTime)
{
	auto & transforms = m_Scene->getSparseSet<CTransform>();
	auto ids = m_Scene->getEntityIDs<CTransform>();
	for( auto id : ids )
	{
		auto & transform = m_Scene->getComponent<CTransform>(id);
		if( transform.position.x < settings::world_boundry_min.x )
		{
			m_Scene->deleteEntity(id);
		}
		if( transform.position.y < settings::world_boundry_min.y )
		{
			m_Scene->deleteEntity(id);
		}
		if( transform.position.z < settings::world_boundry_min.z )
		{
			m_Scene->deleteEntity(id);
		}
		if( transform.position.x > settings::world_boundry_max.x )
		{
			m_Scene->deleteEntity(id);
		}
		if( transform.position.y > settings::world_boundry_max.y )
		{
			m_Scene->deleteEntity(id);
		}
		if( transform.position.z > settings::world_boundry_max.z )
		{
			m_Scene->deleteEntity(id);
		}
	}
}
