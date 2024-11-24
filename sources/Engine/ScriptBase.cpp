#include "ScriptBase.hpp"
#include "BaseScene.hpp"
#include "EventHandler.hpp"

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
	EventHandler::GetInstance()->scriptDispatcher.AddListener(ScriptEvents::Init, [this](const Event<ScriptEvents>& event)
		{
			this->init();
		});
	EventHandler::GetInstance()->scriptDispatcher.AddListener(ScriptEvents::LateInit, [this](const Event<ScriptEvents>& event)
		{
			this->lateInit();
		});

	EventHandler::GetInstance()->scriptDispatcher.AddListener(ScriptEvents::Update,
		[this](const Event<ScriptEvents>& event)
		{
			const auto& eventData = event.ToType<UpdateEvent>();
			this->update(eventData.deltaTime);
		});
	EventHandler::GetInstance()->scriptDispatcher.AddListener(ScriptEvents::LateUpdate,
		[this](const Event<ScriptEvents>& event)
		{
			const auto& eventData = event.ToType<LateUpdateEvent>();
			this->lateUpdate(eventData.deltaTime);
		});
	EventHandler::GetInstance()->scriptDispatcher.AddListener(ScriptEvents::Shutdown, [this](const Event<ScriptEvents>& event)
		{
			this->shutdown();
		});

	// Bind collision events
	EventHandler::GetInstance()->collisionDispatcher.AddListener(CollisionEvents::Enter, [this](const Event<CollisionEvents>& event)
		{
			this->onCollisionEnterEvent(event);
		});
	EventHandler::GetInstance()->collisionDispatcher.AddListener(CollisionEvents::Stay, [this](const Event<CollisionEvents>& event)
		{
			this->onCollisionStayEvent(event);
		});
	EventHandler::GetInstance()->collisionDispatcher.AddListener(CollisionEvents::Exit, [this](const Event<CollisionEvents>& event)
		{
			this->onCollisionExitEvent(event);
		});
}

void ScriptBase::unBindEvents()
{
}

void ScriptBase::onCollisionEnterEvent(const Event<CollisionEvents>& event)
{
	const auto& eventData = event.ToType<CollisionEnterEvent>();
	if( eventData.id1 != mOwnerId && eventData.id2 != mOwnerId )
	{
		return;
	}
	if( eventData.id1 == mOwnerId )
	{
		onCollisionEnter(eventData.id2);
	}
	else
	{
		onCollisionEnter(eventData.id1);
	}
}
void ScriptBase::onCollisionStayEvent(const Event<CollisionEvents>& event)
{
	const auto& eventData = event.ToType<CollisionStayEvent>();
	if( eventData.id1 != mOwnerId && eventData.id2 != mOwnerId )
	{
		return;
	}
	if( eventData.id1 == mOwnerId )
	{
		onCollisionStay(eventData.id2);
	}
	else
	{
		onCollisionStay(eventData.id1);
	}
}

void ScriptBase::onCollisionExitEvent(const Event<CollisionEvents>& event)
{
	const auto& eventData = event.ToType<CollisionExitEvent>();
	if( eventData.id1 != mOwnerId && eventData.id2 != mOwnerId )
	{
		return;
	}
	if( eventData.id1 == mOwnerId )
	{
		onCollisionExit(eventData.id2);
	}
	else
	{
		onCollisionExit(eventData.id1);
	}
}

