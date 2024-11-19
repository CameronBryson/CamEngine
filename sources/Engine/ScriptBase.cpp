#include "ScriptBase.hpp"


ScriptBase::ScriptBase(Registry* registry, unsigned owner_ID)
    : m_Registry(registry), m_OwnerID(owner_ID)
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
