#include "ScriptBase.hpp"
#include "BaseScene.hpp"

ScriptBase::ScriptBase(BaseScene* scene, unsigned short owner_ID)
	: mScene(scene), mOwnerId(owner_ID)
{
	bindEvents();
}

ScriptBase::~ScriptBase()
{
	unBindEvents();
}

void ScriptBase::bindEvents()
{

	EventHandler::GetInstance()->scriptDispatcher.AddListener(ScriptEvents::Init, [this](const Event<ScriptEvents>& event) { this->init(); });
	EventHandler::GetInstance()->scriptDispatcher.AddListener(ScriptEvents::LateInit, [this](const Event<ScriptEvents>& event) { this->lateInit(); });

	EventHandler::GetInstance()->scriptDispatcher.AddListener(ScriptEvents::Update,
	[this](const Event<ScriptEvents>& event)
	{
		auto event_data = static_cast<const UpdateEvent&>(event);
		this->update(event_data.deltaTime);
	});
	EventHandler::GetInstance()->scriptDispatcher.AddListener(ScriptEvents::LateUpdate,
	[this](const Event<ScriptEvents>& event)
	{
		auto event_data = static_cast<const LateUpdateEvent&>(event);
		this->lateUpdate(event_data.deltaTime);
	});
	EventHandler::GetInstance()->scriptDispatcher.AddListener(ScriptEvents::Shutdown, [this](const Event<ScriptEvents>& event) { this->shutdown(); });
}

void ScriptBase::unBindEvents()
{
}

void ScriptBase::onCollisionEnterEvent(const Event<CollisionEvents>& event)
{
	auto event_data = event.ToType<CollisionEnterEvent>();
	if( event_data.id1 != mOwnerId && event_data.id2 != mOwnerId )
	{
		return;
	}
	if( event_data.id1 == mOwnerId )
	{
		onCollisionEnter(event_data.id2);
	}
	else
	{
		onCollisionEnter(event_data.id1);
	}
}
void ScriptBase::onCollisionStayEvent(const Event<CollisionEvents>& event)
{
	auto event_data = event.ToType<CollisionStayEvent>();
	if( event_data.id1 != mOwnerId && event_data.id2 != mOwnerId )
	{
		return;
	}
	if( event_data.id1 == mOwnerId )
	{
		onCollisionStay(event_data.id2);
	}
	else
	{
		onCollisionStay(event_data.id1);
	}
}

void ScriptBase::onCollisionExitEvent(const Event<CollisionEvents>& event)
{
	auto event_data = event.ToType<CollisionExitEvent>();
	if( event_data.id1 != mOwnerId && event_data.id2 != mOwnerId )
	{
		return;
	}
	if( event_data.id1 == mOwnerId )
	{
		onCollisionExit(event_data.id2);
	}
	else
	{
		onCollisionExit(event_data.id1);
	}
}

