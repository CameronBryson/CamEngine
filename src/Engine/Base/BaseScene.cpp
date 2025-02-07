#include "pch.hpp"
#include "BaseScene.hpp"
#include "Engine/Components.hpp"
#include "edyn/edyn.hpp"
BaseScene::BaseScene() : mRenderSystem(this), mAudioSystem(this), mTransformSystem(this)
{
}
void BaseScene::init()
{
	edyn::attach(mEnttRegistry);

	mRenderSystem.init();
	mAudioSystem.init();

}
void BaseScene::lateInit()
{
}
void BaseScene::update(float dt)
{
	edyn::update(mEnttRegistry);
	mTransformSystem.update(dt);
	mAudioSystem.update(dt);
}
void BaseScene::lateUpdate(float dt)
{
}
void BaseScene::render()
{
	mRenderSystem.render();
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
