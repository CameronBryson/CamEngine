#pragma once
#include "Engine/ScriptBase.hpp"
class CameraController : public ScriptBase
{
  public:
	explicit CameraController(BaseScene* scene, unsigned owner_ID) : ScriptBase(scene, owner_ID)
	{
	}
  private:
	void update(float deltaTime) override;
	float camera_movespeed = 100.0f;
	float camera_rotationspeed = 500.0f;
};