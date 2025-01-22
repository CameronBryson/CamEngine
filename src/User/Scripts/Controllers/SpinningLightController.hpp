#pragma once
#include "Engine/Base/ScriptBase.hpp"

class SpinningLightController final : public ScriptBase
{
public:
	SpinningLightController(BaseScene* scene, unsigned short owner_ID) : ScriptBase(scene, owner_ID)
	{
		printf("SpinningLightControllercreated\n");
	}
	void update(float dt) override;

private:
};