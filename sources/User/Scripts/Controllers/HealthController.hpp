#pragma once
#include "Engine/Base/ScriptBase.hpp"

class HealthController : public ScriptBase
{
  public:
	explicit HealthController(BaseScene* scene, unsigned short owner_ID) : ScriptBase(scene, owner_ID)
	{
	}
  private:
	void update(float deltaTime) override;
};
