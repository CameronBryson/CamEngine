#include "pch.hpp"
#include "BaseScene.hpp"
#include "Engine/Components.hpp"
#include "Engine/Events/EventHandler.hpp"
BaseScene::BaseScene() : mRenderSystem(this), mAudioSystem(this)
{
}
void BaseScene::init()
{
	mRenderSystem.init();
	mAudioSystem.init();
	EventHandler::GetInstance()->scriptDispatcher.SendEvent(InitEvent());

}
void BaseScene::lateInit()
{
	mCommandManager.processCommands();
	EventHandler::GetInstance()->scriptDispatcher.SendEvent(LateInitEvent());
}
void BaseScene::update(float dt)
{
	mAudioSystem.update(dt);
	EventHandler::GetInstance()->scriptDispatcher.SendEvent(UpdateEvent(dt));
}
void BaseScene::lateUpdate(float dt)
{
	mCommandManager.processCommands();
	mInputManager.resetKeyStates();
	EventHandler::GetInstance()->scriptDispatcher.SendEvent(LateUpdateEvent(dt));
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
	EventHandler::GetInstance()->scriptDispatcher.SendEvent(ShutdownEvent());
}

void BaseScene::lateShutdown()
{
}
