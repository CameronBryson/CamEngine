#pragma once
#include "Engine/ScriptBase.hpp"
class HealthController : public ScriptBase
{
  public:
    explicit HealthController(Registry* registry, unsigned owner_ID) : ScriptBase(registry, owner_ID)
    {
    }
  private:
    void Update(float deltaTime) override;
};