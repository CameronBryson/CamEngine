#pragma once
#include "Engine/Base/BaseScene.hpp"
#include "User/Scripts/Factory.hpp"

class PlayScene final : public BaseScene
{
public:
    PlayScene();
    ~PlayScene() override;
    void init() override;
	void lateInit() override;
    void update(float dt) override;
    void lateUpdate(float dt) override;
    void render() override;
    void lateRender() override;
    void shutdown() override;
    void lateShutdown() override;

private:
    Factory mFactory;
};
