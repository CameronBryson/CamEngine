#include "CollisionManager.hpp"
#include "Engine/Events/EventHandler.hpp"
#include "Engine/Collision/CollisionManifold.hpp"

CollisionManager::CollisionManager()
{
	auto* eventHandler = EventHandler::GetInstance();

	// Bind collision events and store handles
	mCollisionEventHandles.push_back(eventHandler->collisionDispatcher.AddListener(
	    CollisionEvents::Detected,
	    [this](const Event<CollisionEvents>& event) { handleCollisionDetectedEvent(event); }));

	mCollisionEventHandles.push_back(eventHandler->collisionDispatcher.AddListener(
	    CollisionEvents::NotDetected,
	    [this](const Event<CollisionEvents>& event) { handleCollisionNotDetectedEvent(event); }));

	mCollisionEventHandles.push_back(eventHandler->collisionDispatcher.AddListener(
	    CollisionEvents::ProcessCollisions,
	    [this](const Event<CollisionEvents>& event) { processCollisionResolutions(); }));
}

CollisionManager::~CollisionManager()
{
	// Unbind event handlers
	clearCollisions();
}

void CollisionManager::handleCollisionDetectedEvent(const Event<CollisionEvents>& event)
{
    const auto& newEvent = event.ToType<CollisionDetectedEvent>();
    auto pair = std::make_pair(newEvent.id1, newEvent.id2);
    if (!mCollisionMap.contains(pair))
    {
        printf("Collision Enter\n");
        EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionEnterEvent(newEvent.id1, newEvent.id2));
    }
    else
    {
        printf("Collision Stay\n");
        EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionStayEvent(newEvent.id1, newEvent.id2));
    }
    mCollisionMap[pair] = std::make_unique<CollisionManifold>(newEvent.manifold);
}

void CollisionManager::handleCollisionNotDetectedEvent(const Event<CollisionEvents>& event)
{
    const auto& newEvent = event.ToType<CollisionNotDetectedEvent>();
    auto pair = std::make_pair(newEvent.id1, newEvent.id2);
    if (mCollisionMap.contains(pair))
    {
        printf("Collision Exit\n");
        EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionExitEvent(newEvent.id1, newEvent.id2));
        mCollisionMap.erase(pair);
    }
}


void CollisionManager::processCollisionResolutions()
{
    for (auto& [key, value] : mCollisionMap)
    {
        value->resolveCollision();
        value->penetrationDepth = 0.0f;
    }
}

void CollisionManager::clearCollisions()
{
	auto* eventHandler = EventHandler::GetInstance();

	// Unbind collision events using stored handles
	for (const auto& handle : mCollisionEventHandles)
	{
		eventHandler->collisionDispatcher.RemoveListener(handle);
	}
	mCollisionEventHandles.clear();

	// Optionally clear collision map if needed
	mCollisionMap.clear();
}
