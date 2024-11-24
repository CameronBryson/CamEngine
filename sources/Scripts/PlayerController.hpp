#pragma once
#include "Engine/ScriptBase.hpp"

class PlayerController final : public ScriptBase
{
public:
    explicit PlayerController(BaseScene* scene, unsigned owner_ID) : ScriptBase(scene, owner_ID)
    {
	    printf("PlayerController created\n");
    }
  private:
    // Inherited via ScriptBase
    void update(float deltaTime) override;
private:
    //should probably be in player class
    float movespeed = 10.0f;
    float rotation_speed = 0.25f;
};