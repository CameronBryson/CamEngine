#pragma once
#include "ScriptBase.hpp"
class DamageController : public ScriptBase
{
  public:
	explicit DamageController(Registry* registry, unsigned owner_ID) : ScriptBase(registry, owner_ID)
	{
	}
  private:
	void Update(float deltaTime) override;
};