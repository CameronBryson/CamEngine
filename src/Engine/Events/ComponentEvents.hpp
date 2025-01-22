#pragma once
#include "Engine/Events/Event.hpp"
enum class ComponentEvents
{
	Added,
	Removed
};
class ComponentAddedEvent final : public Event<ComponentEvents>
{
public:
	explicit ComponentAddedEvent(unsigned short id) : Event<ComponentEvents>(ComponentEvents::Added, "ComponentAdded"), id(id) {}
	unsigned short id;
};

class ComponentRemovedEvent final : public Event<ComponentEvents>
{
public:
	explicit ComponentRemovedEvent(unsigned short id) : Event<ComponentEvents>(ComponentEvents::Removed, "ComponentRemoved"), id(id) {}
	unsigned short id;
};