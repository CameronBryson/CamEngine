#pragma once
#include "Engine/Collision/CollisionManifold.hpp"
#include "Event.hpp"
enum class CollisionEvents
{
	Detected,
	NotDetected,
	Enter,
	Stay,
	Exit,
	ProcessCollisions
};
class CollisionDetectedEvent final : public Event<CollisionEvents>
{
public:
	explicit CollisionDetectedEvent(const CollisionManifold & manifold, const unsigned short id1, const unsigned short id2) : Event<CollisionEvents>(CollisionEvents::Detected, "CollisionDetected"), manifold(manifold), id1(id1), id2(id2) {}
	CollisionManifold manifold;
	unsigned short id1;
	unsigned short id2;
};
class CollisionNotDetectedEvent final : public Event<CollisionEvents>
{
public:
	explicit CollisionNotDetectedEvent(const unsigned short id1, const unsigned short id2) : Event<CollisionEvents>(CollisionEvents::NotDetected, "CollisionNotDetected"), id1(id1), id2(id2) {}
	unsigned short id1;
	unsigned short id2;
};
class CollisionEnterEvent final : public Event<CollisionEvents>
{
public:
	explicit CollisionEnterEvent(const unsigned short id1, const unsigned short id2) : Event<CollisionEvents>(CollisionEvents::Enter, "CollisionEnter"), id1(id1), id2(id2) {}
	unsigned short id1;
	unsigned short id2;
};
class CollisionStayEvent final : public Event<CollisionEvents>
{
public:
	explicit CollisionStayEvent(const unsigned short id1, const unsigned short id2) : Event<CollisionEvents>(CollisionEvents::Stay, "CollisionStay"), id1(id1), id2(id2) {}
	unsigned short id1;
	unsigned short id2;

};
class CollisionExitEvent final : public Event<CollisionEvents>
{
public:
	explicit CollisionExitEvent(unsigned short id1, unsigned short id2) : Event<CollisionEvents>(CollisionEvents::Exit, "CollisionExit"), id1(id1),id2(id2) {}
	unsigned short id1;
	unsigned short id2;
};
class ProcessCollisionEvent final : public Event<CollisionEvents>
{
public:
	explicit ProcessCollisionEvent() : Event<CollisionEvents>(CollisionEvents::ProcessCollisions, "ProcessCollisions") {}
};
