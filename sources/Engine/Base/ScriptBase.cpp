#include "Engine/pch.hpp"
#include "ScriptBase.hpp"

#include "BaseScene.hpp"
#include "Engine/Events/EventHandler.hpp"
#include <vector>
ScriptBase::ScriptBase(BaseScene* scene, unsigned short owner_ID) : mScene(scene), mOwnerId(owner_ID) { bindEvents(); }

ScriptBase::~ScriptBase()
{ 
	unBindEvents(); 
}

void ScriptBase::bindEvents()
{

	// Bind script events
	mScriptEventHandles.push_back(EventHandler::GetInstance()->scriptDispatcher.AddListener(
	    ScriptEvents::Init, [this](const Event<ScriptEvents>& event) { this->init(); }));

	mScriptEventHandles.push_back(EventHandler::GetInstance()->scriptDispatcher.AddListener(
	    ScriptEvents::LateInit, [this](const Event<ScriptEvents>& event) { this->lateInit(); }));

	mScriptEventHandles.push_back(EventHandler::GetInstance()->scriptDispatcher.AddListener(
	    ScriptEvents::Update,
	                                                                         [this](const Event<ScriptEvents>& event)
	                                                                         {
		                                                                         const auto& eventData =
		                                                                             event.ToType<UpdateEvent>();
		                                                                         this->update(eventData.deltaTime);
	                                                                         }));

	mScriptEventHandles.push_back(EventHandler::GetInstance()->scriptDispatcher.AddListener(
	    ScriptEvents::LateUpdate,
	                                                                         [this](const Event<ScriptEvents>& event)
	                                                                         {
		                                                                         const auto& eventData =
		                                                                             event.ToType<LateUpdateEvent>();
		                                                                         this->lateUpdate(eventData.deltaTime);
	                                                                         }));

	mScriptEventHandles.push_back(EventHandler::GetInstance()->scriptDispatcher.AddListener(
	    ScriptEvents::Shutdown, [this](const Event<ScriptEvents>& event) { this->shutdown(); }));

	// Bind collision events
	mCollisionEventHandles.push_back(EventHandler::GetInstance()->collisionDispatcher.AddListener(
	    CollisionEvents::Enter, [this](const Event<CollisionEvents>& event) { this->onCollisionEnterEvent(event); }));

	mCollisionEventHandles.push_back(EventHandler::GetInstance()->collisionDispatcher.AddListener(
	    CollisionEvents::Stay, [this](const Event<CollisionEvents>& event) { this->onCollisionStayEvent(event); }));

	mCollisionEventHandles.push_back(EventHandler::GetInstance()->collisionDispatcher.AddListener(
	    CollisionEvents::Exit, [this](const Event<CollisionEvents>& event) { this->onCollisionExitEvent(event); }));
}

void ScriptBase::unBindEvents()
{

	// Unbind script events
	for (const auto& handle : mScriptEventHandles)
	{
		EventHandler::GetInstance()->scriptDispatcher.RemoveListener(handle);
	}
	mScriptEventHandles.clear();

	// Unbind collision events
	for (const auto& handle : mCollisionEventHandles)
	{
		EventHandler::GetInstance()->collisionDispatcher.RemoveListener(handle);
	}
	mCollisionEventHandles.clear();
}
void ScriptBase::onCollisionEnterEvent(const Event<CollisionEvents>& event)
{
	const auto& eventData = event.ToType<CollisionEnterEvent>();
	onCollisionEnter(eventData.id1, eventData.id2);
}
// void ScriptBase::onCollisionEnterEvent(const Event<CollisionEvents>& event)
//{
//	const auto& eventData = event.ToType<CollisionEnterEvent>();
//	if( eventData.id1 != mOwnerId && eventData.id2 != mOwnerId )
//	{
//		return;
//	}
//	if( eventData.id1 == mOwnerId )
//	{
//		onCollisionEnter(eventData.id2);
//	}
//	else
//	{
//		onCollisionEnter(eventData.id1);
//	}
// }
void ScriptBase::onCollisionStayEvent(const Event<CollisionEvents>& event)
{
	const auto& eventData = event.ToType<CollisionStayEvent>();
	onCollisionStay(eventData.id1, eventData.id2);
}
// void ScriptBase::onCollisionStayEvent(const Event<CollisionEvents>& event)
//{
//	const auto& eventData = event.ToType<CollisionStayEvent>();
//	if( eventData.id1 != mOwnerId && eventData.id2 != mOwnerId )
//	{
//		return;
//	}
//	if( eventData.id1 == mOwnerId )
//	{
//		onCollisionStay(eventData.id2);
//	}
//	else
//	{
//		onCollisionStay(eventData.id1);
//	}
// }
void ScriptBase::onCollisionExitEvent(const Event<CollisionEvents>& event)
{
	const auto& eventData = event.ToType<CollisionExitEvent>();
	onCollisionExit(eventData.id1, eventData.id2);
}
// void ScriptBase::onCollisionExitEvent(const Event<CollisionEvents>& event)
//{
//	const auto& eventData = event.ToType<CollisionExitEvent>();
//	if( eventData.id1 != mOwnerId && eventData.id2 != mOwnerId )
//	{
//		return;
//	}
//	if( eventData.id1 == mOwnerId )
//	{
//		onCollisionExit(eventData.id2);
//	}
//	else
//	{
//		onCollisionExit(eventData.id1);
//	}
// }
