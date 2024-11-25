#include "BaseScene.hpp"
#include "Engine/Components.hpp"
#include "Engine/Events/EventHandler.hpp"
BaseScene::BaseScene() : mCollisionSystem(this), mPhysicsSystem(this), mRenderSystem(this), mAudioSystem(this)
{
}
void BaseScene::init()
{
	initSpaseSets();
	mRenderSystem.init();
	mCollisionSystem.init();
	mPhysicsSystem.init();
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
	mPhysicsSystem.update(dt);
	mAudioSystem.update(dt);
	EventHandler::GetInstance()->scriptDispatcher.SendEvent(UpdateEvent(dt));
}
void BaseScene::lateUpdate(float dt)
{
	mCollisionSystem.update();
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
	mPhysicsSystem.shutdown();
	mCollisionSystem.shutdown();
	EventHandler::GetInstance()->scriptDispatcher.SendEvent(ShutdownEvent());
}

void BaseScene::lateShutdown()
{
}

unsigned short BaseScene::createEntity()
{
	return mRegistry.createEntity();
}
void BaseScene::deleteEntity(unsigned short id)
{
	mRegistry.deleteEntity(id);
}
KeyAction BaseScene::getKeyAction(int key)
{
	return mInputManager.getKeyAction(key);
}
void BaseScene::initSpaseSets()
{
    mRegistry.createSparseSet<CTransform>();
    mRegistry.createSparseSet<CRender>();
    mRegistry.createSparseSet<CCollider>();
    mRegistry.createSparseSet<CBoxBounds>();
    mRegistry.createSparseSet<CSphereBounds>();
    mRegistry.createSparseSet<CModel>();
    mRegistry.createSparseSet<CDirectionalLight>();
    mRegistry.createSparseSet<CDynamicBody>();
    mRegistry.createSparseSet<CUI>();
    mRegistry.createSparseSet<CBackground>();
    mRegistry.createSparseSet<CPointLight>();
    mRegistry.createSparseSet<CRepeatAcceleration>();
}
