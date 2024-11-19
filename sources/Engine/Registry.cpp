#include "Registry.hpp"
#include "Scripts/Health.hpp"
#include "Scripts/Damage.hpp"

Registry::Registry()
{
    printf("Registry created\n");
    mEntities.reserve(settings::max_entities);
    mFreeIDs.resize(settings::max_entities - 1);
    std::iota(mFreeIDs.begin(), mFreeIDs.end(), 1);
    for( int i = 0; i < 512; ++i )
    {
	mKeyMap[i] = KeyAction::None;
    }
    EventHandler::GetInstance()->collisionDispatcher.AddListener(CollisionEvents::Detected,
	std::bind(&Registry::onCollisionDetectedEvent, this, std::placeholders::_1));
    EventHandler::GetInstance()->collisionDispatcher.AddListener(CollisionEvents::NotDetected,
	std::bind(&Registry::onCollisionNotDetectedEvent, this, std::placeholders::_1));
    EventHandler::GetInstance()->collisionDispatcher.AddListener(CollisionEvents::Enter,
	std::bind(&Registry::onCollisionEnterEvent, this, std::placeholders::_1));
    EventHandler::GetInstance()->collisionDispatcher.AddListener(CollisionEvents::Exit,
	std::bind(&Registry::onCollisionExitEvent, this, std::placeholders::_1));
    EventHandler::GetInstance()->inputDispatcher.AddListener(InputEvents::KeyPress, std::bind(&Registry::onInputPressEvent, this, std::placeholders::_1));
    EventHandler::GetInstance()->inputDispatcher.AddListener(InputEvents::KeyRelease, std::bind(&Registry::onInputReleaseEvent, this, std::placeholders::_1));
}

Registry::~Registry()
{
    processCommands();
    //remove all event bindings
    printf("Registry destroyed\n");
}

unsigned short Registry::createEntity()
{
    assert(! mFreeIDs.empty() && "No more entities available.");
    const unsigned short id = mFreeIDs.back();
    mFreeIDs.pop_back();
    mEntities.emplace_back(id);
    return id;
}

void Registry::deleteEntity(unsigned short id)
{
    mCommandQueue.push(std::make_unique<DeleteEntityCommand>(mSparseSets, mFreeIDs, mEntities, id));
}

void Registry::processCommands()
{
    while( ! mCommandQueue.empty() )
    {
	const auto& command = mCommandQueue.front();
	command->execute();
	mCommandQueue.pop();
    }
}

void Registry::onCollisionDetectedEvent(const Event<CollisionEvents>& event)
{
    auto new_event = event.ToType<CollisionDetectedEvent>();
    auto pair = std::make_pair(new_event.id1, new_event.id2);
    if( ! mCollisionMap.contains(pair) )
    {
	//printf("Collision Enter\n");
	EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionEnterEvent(new_event.id1, new_event.id2));
    }
    else
    {
	//printf("Collision Stay\n");
	EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionStayEvent(new_event.id1, new_event.id2));
    }
    mCollisionMap[pair] = std::make_unique<CollisionManifold>(new_event.manifold);
}

void Registry::onCollisionNotDetectedEvent(const Event<CollisionEvents>& event)
{
    auto new_event = event.ToType<CollisionNotDetectedEvent>();
    auto pair = std::make_pair(new_event.id1, new_event.id2);
    if( mCollisionMap.contains(pair) )
    {
	//printf("Collision Exit\n");
	EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionExitEvent(new_event.id1, new_event.id2));
	mCollisionMap.erase(pair);
    }
}

void Registry::onCollisionEnterEvent(const Event<CollisionEvents>& event)
{
    auto new_event = event.ToType<CollisionEnterEvent>();
    bool has_health1 = hasComponent<Health>(new_event.id1);
    bool has_health2 = hasComponent<Health>(new_event.id2);
    bool has_damage1 = hasComponent<Damage>(new_event.id1);
    bool has_damage2 = hasComponent<Damage>(new_event.id2);

    if( has_health1 && has_damage2 )
    {
	auto& health = getComponent<Health>(new_event.id1);
	auto& damage = getComponent<Damage>(new_event.id2);
	health.health -= damage.damage;
    }

    if( has_health2 && has_damage1 )
    {
	auto& health = getComponent<Health>(new_event.id2);
	auto& damage = getComponent<Damage>(new_event.id1);
	health.health -= damage.damage;
    }
}

void Registry::onCollisionExitEvent(const Event<CollisionEvents>& event)
{
    auto new_event = event.ToType<CollisionExitEvent>();
}

void Registry::onInputPressEvent(const Event<InputEvents>& event)
{
    auto event_data = event.ToType<KeyPressEvent>();
    if( mKeyMap[event_data.key] == KeyAction::None )
    {
	//EventHandler::GetInstance()->input_dispatcher.SendEvent(KeyStartEvent(event_data.key));
	mKeyMap[event_data.key] = KeyAction::Start;
    }
    else
    {
	mKeyMap[event_data.key] = KeyAction::Hold;
    }
}

void Registry::onInputReleaseEvent(const Event<InputEvents>& event)
{
    auto event_data = event.ToType<KeyRelease>();
    if( mKeyMap[event_data.key] == KeyAction::Start || mKeyMap[event_data.key] == KeyAction::Hold )
    {
	//EventHandler::GetInstance()->input_dispatcher.SendEvent(KeyEndEvent(event_data.key));
	mKeyMap[event_data.key] = KeyAction::End;
    }
    else
    {
	mKeyMap[event_data.key] = KeyAction::None;
	//this isnt working cause there isnt a new event being sent
    }
}

void Registry::processCollisionResolutions()
{
    for( auto& [key, value] : mCollisionMap )
    {
	value->resolveCollision();
	value->penetrationDepth = 0.0f;
    }
}

KeyAction Registry::getKeyAction(int key)
{
    return mKeyMap[key];
}

void Registry::resetKeyStates()
{
    for( auto& [key, action] : mKeyMap )
    {
	if( action == KeyAction::Start )
	{
	    action = KeyAction::Hold;
	}
	else if( action == KeyAction::End )
	{
	    action = KeyAction::None;
	}
    }
}
