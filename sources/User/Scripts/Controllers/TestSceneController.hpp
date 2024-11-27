#pragma once
#include "Engine/Base/BaseScene.hpp"
#include "Engine/Base/ScriptBase.hpp"
#include "Engine/Graphics/Camera.hpp"
class TestSceneController final : public ScriptBase
{
public:
	explicit TestSceneController(BaseScene* scene, unsigned short owner_ID) : ScriptBase(scene, owner_ID)
	{
		printf("TestSceneController created\n");
	}
	void update(float deltaTime) override;
private:
	Camera* m_Camera;
};