#pragma once
#include "Engine/ScriptBase.hpp"
class AsteroidController : public ScriptBase
{
  public:
    explicit AsteroidController(BaseScene* scene, unsigned owner_ID) : ScriptBase(scene, owner_ID)
    {
	printf("AsteroidController created\n");
    }
  private:
    void update(float deltaTime) override;
};