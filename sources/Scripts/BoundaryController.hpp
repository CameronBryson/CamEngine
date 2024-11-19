#pragma once
#include "Engine/ScriptBase.hpp"
class BoundaryController : public ScriptBase
{
  public:
    explicit BoundaryController(Registry* registry, unsigned owner_ID) : ScriptBase(registry, owner_ID)
    {
    }
  private:
    void Update(float deltaTime) override;
};