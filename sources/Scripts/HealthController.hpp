#pragma once
#include "Engine/ScriptBase.hpp"
class HealthController : public ScriptBase
{
  public:
	explicit HealthController(BaseScene* scene, unsigned owner_ID) : ScriptBase(scene, owner_ID)
	{
	}
  private:
	void update(float deltaTime) override;
};