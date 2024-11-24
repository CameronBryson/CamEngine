#pragma once
#include "Engine/ScriptBase.hpp"
class DamageController : public ScriptBase
{
  public:
	explicit DamageController(BaseScene* scene, unsigned owner_ID) : ScriptBase(scene, owner_ID)
	{
	}
  private:
	void update(float deltaTime) override;
};