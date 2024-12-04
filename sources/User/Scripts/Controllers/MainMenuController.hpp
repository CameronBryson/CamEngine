#pragma once
#include "Engine/Base/ScriptBase.hpp"
class MainMenuController final : public ScriptBase
{
public:
	MainMenuController(BaseScene* scene, unsigned short owner_ID) : ScriptBase(scene, owner_ID)
	{
		printf("MainMenuController created\n");
	}
	void update(float deltaTime) override;

private:
};