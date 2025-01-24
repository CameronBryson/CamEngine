#include "pch.hpp"
#include "ScriptBase.hpp"

#include "BaseScene.hpp"
#include "Engine/Events/EventHandler.hpp"
#include <vector>
#include <Event.hpp>
#include <ScriptEvents.hpp>
#include <entt/entity/fwd.hpp>
ScriptBase::ScriptBase(BaseScene* scene, entt::entity owner_ID) : mScene(scene), mOwnerId(owner_ID) { bindEvents(); }

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

}

void ScriptBase::unBindEvents()
{

	// Unbind script events
	for (const auto& handle : mScriptEventHandles)
	{
		EventHandler::GetInstance()->scriptDispatcher.RemoveListener(handle);
	}
	mScriptEventHandles.clear();

}
