#pragma once
#include "Engine/ScriptBase.hpp"
class CameraController : public ScriptBase
{
  public:
    explicit CameraController(Registry* registry, unsigned owner_ID) : ScriptBase(registry, owner_ID)
    {
    }
  private:
    void Update(float deltaTime) override;
};