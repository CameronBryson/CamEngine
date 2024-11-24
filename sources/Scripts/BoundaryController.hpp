#pragma once
#include "Engine/ScriptBase.hpp"
class BoundaryController : public ScriptBase
{
  public:
	explicit BoundaryController(BaseScene* scene, unsigned owner_ID) : ScriptBase(scene, owner_ID)
	{
	}
  private:
	void Update(float deltaTime) override;
};