#pragma once
#include "ScriptBase.hpp"
class DamageController : public ScriptBase
{
  public:
	explicit DamageController(BaseScene* scene, unsigned owner_ID) : ScriptBase(scene, owner_ID)
	{
	}
  private:
	void Update(float deltaTime) override;
};