#pragma once
#include "BaseScene.hpp"

class TestScene final : public BaseScene
{
public:
	TestScene();
	~TestScene() override;
	void init() override;
	void lateInit() override;
	void update(float dt) override;
	void lateUpdate(float dt) override;
	void render(float dt) override;
	void lateRender() override;
	void shutdown() override;
	void lateShutdown() override;
private:
	float mMoveSpeed = 5.0f;          // Units per second
	float mMouseSensitivity = 0.1f;   // Degrees per pixel
	bool mIsRightMousePressed = false;
	double mLastMouseX = 0.0, mLastMouseY = 0.0;
};