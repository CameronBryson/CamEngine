#pragma once
#include "Engine/Base/ScriptBase.hpp"

class DamageController : public ScriptBase
{
  public:
	explicit DamageController(BaseScene* scene, unsigned short owner_ID) : ScriptBase(scene, owner_ID)
	{
	}
  private:
	void update(float deltaTime) override;
};
