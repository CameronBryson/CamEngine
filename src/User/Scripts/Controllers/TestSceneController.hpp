#pragma once
#include "Engine/Base/BaseScene.hpp"
#include "Engine/Base/ScriptBase.hpp"
#include "Engine/Graphics/Camera.hpp"
class TestSceneController final : public ScriptBase
{
public:
	explicit TestSceneController(BaseScene* scene, entt::entity owner_ID);
	void init() override;
	void update(float deltaTime) override;
private:
	Camera* m_Camera;
	float camera_movespeed = 10.0f;
};