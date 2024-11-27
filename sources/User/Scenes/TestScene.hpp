#pragma once
#include "Engine/Base/BaseScene.hpp"

class TestScene final : public BaseScene
{
public:
	TestScene();
	~TestScene() override;
	void init() override;
	void lateInit() override;
	void update(float dt) override;
	void lateUpdate(float dt) override;
	void render() override;
	void lateRender() override;
	void shutdown() override;
	void lateShutdown() override;
private:
};