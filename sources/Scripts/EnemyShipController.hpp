#pragma once
#include "Engine/ScriptBase.hpp"
class EnemyShipController : public ScriptBase
{
  public:
    explicit EnemyShipController(Registry* registry, unsigned owner_ID) : ScriptBase(registry, owner_ID)
    {
    }
  private:
    void Update(float deltaTime) override;
};