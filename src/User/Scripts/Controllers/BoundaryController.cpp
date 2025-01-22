#include "pch.hpp"
#include "BoundaryController.hpp"

#include "Engine/Util/GameSettings.hpp"
#include "Engine/Base/BaseScene.hpp"

void BoundaryController::update(float deltaTime)
{
	auto & transforms = GetScene().getSparseSet<CTransform>();
	auto ids = GetScene().getEntityIDs<CTransform>();
	for( auto id : ids )
	{
		auto & transform = GetScene().getComponent<CTransform>(id);
		if( transform.position.x < settings::world_boundry_min.x )
		{
			GetScene().deleteEntity(id);
		}
		if( transform.position.y < settings::world_boundry_min.y )
		{
			GetScene().deleteEntity(id);
		}
		if( transform.position.z < settings::world_boundry_min.z )
		{
			GetScene().deleteEntity(id);
		}
		if( transform.position.x > settings::world_boundry_max.x )
		{
			GetScene().deleteEntity(id);
		}
		if( transform.position.y > settings::world_boundry_max.y )
		{
			GetScene().deleteEntity(id);
		}
		if( transform.position.z > settings::world_boundry_max.z )
		{
			GetScene().deleteEntity(id);
		}
	}
}
