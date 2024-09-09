#pragma once
#include "Collision.hpp"
#include "Event.hpp"
enum class CollisionEvents
{
    Enter,
    Exit
};
class CollisionEnterEvent final : public Event<CollisionEvents>
{
public:
    explicit CollisionEnterEvent(collision_manifold & manifold) : Event<CollisionEvents>(CollisionEvents::Enter, "CollisionEnter"), manifold(manifold) {};
    collision_manifold& manifold;
};
class CollisionExitEvent final : public Event<CollisionEvents>
{
public:
    explicit CollisionExitEvent(collision_manifold & manifold) : Event<CollisionEvents>(CollisionEvents::Exit, "CollisionExit"), manifold(manifold) {};
    collision_manifold& manifold;
};
