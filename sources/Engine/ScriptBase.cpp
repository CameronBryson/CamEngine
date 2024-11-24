#include "ScriptBase.hpp"


ScriptBase::ScriptBase(BaseScene* scene, unsigned owner_ID)
	: m_Scene(scene), m_OwnerID(owner_ID)
{
	BindEvents();
}

ScriptBase::~ScriptBase()
{
	UnBindEvents();
}

void ScriptBase::BindEvents()
{

	EventHandler::GetInstance()->scriptDispatcher.AddListener(ScriptEvents::Init, [this](const Event<ScriptEvents>& event) { this->Init(); });
	EventHandler::GetInstance()->scriptDispatcher.AddListener(ScriptEvents::LateInit, [this](const Event<ScriptEvents>& event) { this->LateInit(); });

	EventHandler::GetInstance()->scriptDispatcher.AddListener(ScriptEvents::Update,
	[this](const Event<ScriptEvents>& event)
	{
		auto event_data = static_cast<const UpdateEvent&>(event);
		this->Update(event_data.deltaTime);
	});
	EventHandler::GetInstance()->scriptDispatcher.AddListener(ScriptEvents::LateUpdate,
	[this](const Event<ScriptEvents>& event)
	{
		auto event_data = static_cast<const LateUpdateEvent&>(event);
		this->LateUpdate(event_data.deltaTime);
	});
	EventHandler::GetInstance()->scriptDispatcher.AddListener(ScriptEvents::Shutdown, [this](const Event<ScriptEvents>& event) { this->Shutdown(); });
}

void ScriptBase::UnBindEvents()
{
}

void ScriptBase::OnCollisionEnterEvent(const Event<CollisionEvents>& event)
{
	auto event_data = event.ToType<CollisionEnterEvent>();
	if( event_data.id1 != m_OwnerID && event_data.id2 != m_OwnerID )
	{
		return;
	}
	if( event_data.id1 == m_OwnerID )
	{
		OnCollisionEnter(event_data.id2);
	}
	else
	{
		OnCollisionEnter(event_data.id1);
	}
}
void ScriptBase::OnCollisionStayEvent(const Event<CollisionEvents>& event)
{
	auto event_data = event.ToType<CollisionStayEvent>();
	if( event_data.id1 != m_OwnerID && event_data.id2 != m_OwnerID )
	{
		return;
	}
	if( event_data.id1 == m_OwnerID )
	{
		OnCollisionStay(event_data.id2);
	}
	else
	{
		OnCollisionStay(event_data.id1);
	}
}

void ScriptBase::OnCollisionExitEvent(const Event<CollisionEvents>& event)
{
	auto event_data = event.ToType<CollisionExitEvent>();
	if( event_data.id1 != m_OwnerID && event_data.id2 != m_OwnerID )
	{
		return;
	}
	if( event_data.id1 == m_OwnerID )
	{
		OnCollisionExit(event_data.id2);
	}
	else
	{
		OnCollisionExit(event_data.id1);
	}
}

