#pragma once
#include "Engine/Base/ScriptBase.hpp"
#include "KeyAction.hpp"

class PlayerController final : public ScriptBase
{
public:
	explicit PlayerController(BaseScene* scene, entt::entity owner_ID) : ScriptBase(scene, owner_ID)
	{
		printf("PlayerController created\n");
	}
  private:
	// Inherited via ScriptBase
	void update(float deltaTime) override;
private:
	//should probably be in player class
	float moveSpeed = 20.0f;
	float rotation_speed = 0.25f;
};
