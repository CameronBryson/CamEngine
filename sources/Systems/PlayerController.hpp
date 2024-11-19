#pragma once
#include "Engine/ScriptBase.hpp"
#include "Engine/Registry.hpp"
#include "Graphics/Camera.hpp"

class PlayerController final : public ScriptBase
{
public:
    PlayerController(Camera* camera, Registry* registry, unsigned owner_ID) : ScriptBase(registry, owner_ID), m_Camera(camera)
    {
	    printf("PlayerController created\n");
    }
  private:
    // Inherited via ScriptBase
    void Init() override;
    void LateInit() override;
    void Update(float deltaTime) override;
    void LateUpdate(float deltaTime) override;
    void Shutdown() override;
private:
    float movespeed = 10.0f;
    float rotation_speed = 0.25f;
    Camera* m_Camera;
};