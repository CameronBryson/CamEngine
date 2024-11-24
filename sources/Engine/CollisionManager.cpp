#include "CollisionManager.hpp"
#include "EventHandler.hpp"

CollisionManager::CollisionManager()
{
	EventHandler::GetInstance()->collisionDispatcher.AddListener(CollisionEvents::Detected, std::bind(&CollisionManager::handleCollisionDetectedEvent, this, std::placeholders::_1));
	EventHandler::GetInstance()->collisionDispatcher.AddListener(CollisionEvents::NotDetected, std::bind(&CollisionManager::handleCollisionNotDetectedEvent, this, std::placeholders::_1));
	EventHandler::GetInstance()->collisionDispatcher.AddListener(CollisionEvents::ProcessCollisions, std::bind(&CollisionManager::processCollisionResolutions, this));
}

CollisionManager::~CollisionManager()
{
	//unbind event handlers
    clearCollisions();
}

void CollisionManager::handleCollisionDetectedEvent(const Event<CollisionEvents>& event)
{
    auto new_event = event.ToType<CollisionDetectedEvent>();
    auto pair = std::make_pair(new_event.id1, new_event.id2);
    if (!mCollisionMap.contains(pair))
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

void CollisionManager::handleCollisionNotDetectedEvent(const Event<CollisionEvents>& event)
{
    auto new_event = event.ToType<CollisionNotDetectedEvent>();
    auto pair = std::make_pair(new_event.id1, new_event.id2);
    if (mCollisionMap.contains(pair))
    {
        //printf("Collision Exit\n");
        EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionExitEvent(new_event.id1, new_event.id2));
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
}
