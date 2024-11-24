#pragma once
#include "Engine/Base/ScriptBase.hpp"

class HealthController : public ScriptBase
{
  public:
	explicit HealthController(BaseScene* scene, unsigned short owner_ID) : ScriptBase(scene, owner_ID)
	{
	}
  private:
	void onCollisionEnter(unsigned short firstID, unsigned short secondID) override;
	void update(float deltaTime) override;
};
