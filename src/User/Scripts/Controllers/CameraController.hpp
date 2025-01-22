#pragma once
#include "Engine/Base/ScriptBase.hpp"
#include <entt/entity/fwd.hpp>

class CameraController : public ScriptBase
{
  public:
	  explicit CameraController(BaseScene* scene, entt::entity owner_ID) : ScriptBase(scene, owner_ID)
	{
	}
  private:
	void update(float deltaTime) override;
	float camera_movespeed = 1000.0f;
	float camera_rotationspeed = 500.0f;
};
