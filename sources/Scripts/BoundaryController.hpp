#pragma once
#include "Engine/ScriptBase.hpp"
class BoundaryController : public ScriptBase
{
  public:
	explicit BoundaryController(BaseScene* scene, unsigned short owner_ID) : ScriptBase(scene, owner_ID)
	{
	}
  private:
	void update(float deltaTime) override;
};