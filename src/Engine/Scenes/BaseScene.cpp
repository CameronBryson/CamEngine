#include "pch.hpp"
#include "BaseScene.hpp"
#include "Engine/Components.hpp"
BaseScene::BaseScene() : mRenderSystem(this), mTransformSystem(this)
{
}
void BaseScene::init()
{

	mRenderSystem.init();

}
void BaseScene::lateInit()
{
	mRenderSystem.lateInit();
}
void BaseScene::update(float dt)
{
	mTransformSystem.update(dt);
}
void BaseScene::lateUpdate(float dt)
{
}
void BaseScene::render(float dt)
{
	mRenderSystem.render(dt);
}

void BaseScene::lateRender()
{
}

void BaseScene::shutdown()
{
	mRenderSystem.shutdown();
}

void BaseScene::lateShutdown()
{
}
