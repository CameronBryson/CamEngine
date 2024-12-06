#include "Engine/pch.hpp"
#include "SpinningLightController.hpp"
#include "Engine/Base/BaseScene.hpp"
void SpinningLightController::update(float dt)
{
	const auto modelIds = GetScene().mRegistry.getEntityIDs<CPointLight, CTransform>();
	for (const auto id : modelIds)
	{
		auto& transform = GetScene().mRegistry.getComponent<CTransform>(id);
		transform.rotation.y += 1.0f * dt;
	}
}