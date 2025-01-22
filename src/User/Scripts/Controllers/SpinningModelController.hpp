#pragma once
#include "Engine/Base/ScriptBase.hpp"

class SpinningModelController final : public ScriptBase
{
public:
	SpinningModelController(BaseScene* scene, unsigned short owner_ID) : ScriptBase(scene, owner_ID)
	{
		printf("SpinningModelControllercreated\n");
	}
	void update(float dt) override;

private:
};