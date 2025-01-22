#pragma once
#include <cstdio>

#include "Engine/Base/ScriptBase.hpp"

class AsteroidController : public ScriptBase
{
  public:
	explicit AsteroidController(BaseScene* scene, unsigned short owner_ID) : ScriptBase(scene, owner_ID)
	{
	printf("AsteroidController created\n");
	}
  private:
	void update(float deltaTime) override;
};
