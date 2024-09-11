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
    explicit CollisionEnterEvent(collision_manifold & manifold, unsigned short id1, unsigned short id2) : Event<CollisionEvents>(CollisionEvents::Enter, "CollisionEnter"), manifold(manifold), id1(id1), id2(id2) {};
    collision_manifold& manifold;
    unsigned short id1;
    unsigned short id2;
};
class CollisionExitEvent final : public Event<CollisionEvents>
{
public:
    explicit CollisionExitEvent(unsigned short id1, unsigned short id2) : Event<CollisionEvents>(CollisionEvents::Exit, "CollisionExit"), id1(id1),id2(id2) {};
    unsigned short id1;
    unsigned short id2;
};
