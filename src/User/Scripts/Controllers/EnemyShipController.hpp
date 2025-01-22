#pragma once
#include "Engine/Base/ScriptBase.hpp"

class EnemyShipController : public ScriptBase
{
  public:
    explicit EnemyShipController(BaseScene* scene, unsigned short owner_ID) : ScriptBase(scene, owner_ID)
    {
    }
  private:
    void update(float deltaTime) override;
};
