#include "SpinningModelController.hpp"
#include "Engine/Base/BaseScene.hpp"
void SpinningModelController::update(float dt)
{ 
	const auto modelIds = GetScene().mRegistry.getEntityIDs<CModel, CTransform>();
	for (const auto id : modelIds)
	{
		auto& transform = GetScene().mRegistry.getComponent<CTransform>(id);
		transform.rotation.y += 1.0f * dt;
	}
}