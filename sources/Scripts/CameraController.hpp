#pragma once
#include "Engine/ScriptBase.hpp"
#include "Graphics/Camera.hpp"
class CameraController : public ScriptBase
{
  public:
	explicit CameraController(Camera* camera, Registry* registry, unsigned owner_ID) : ScriptBase(registry, owner_ID), m_Camera(camera)
	{
	}
  private:
	void Update(float deltaTime) override;
	Camera* m_Camera;
	float camera_movespeed = 100.0f;
	float camera_rotationspeed = 500.0f;
};