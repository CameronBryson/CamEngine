#pragma once
#include "Engine/ScriptBase.hpp"
class EnemyShipController : public ScriptBase
{
  public:
    explicit EnemyShipController(BaseScene* scene, unsigned owner_ID) : ScriptBase(scene, owner_ID)
    {
    }
  private:
    void update(float deltaTime) override;
};