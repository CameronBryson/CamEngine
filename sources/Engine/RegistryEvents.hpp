#pragma once
#include "Event.hpp"
enum class RegistryEvents
{
    CreateEntity,
    DeleteEntity,
    AddComponent,
    RemoveComponent
};
class CreateEntityEvent final : public Event<RegistryEvents> {
public:
    explicit CreateEntityEvent(unsigned short id) : Event<RegistryEvents>(RegistryEvents::CreateEntity, "CreateEntity"), id(id) {};
    unsigned short id;
private:
};
class DeleteEntityEvent final : public Event<RegistryEvents> {
public:
    explicit DeleteEntityEvent(unsigned short id) : Event<RegistryEvents>(RegistryEvents::DeleteEntity, "DeleteEntity"), id(id) {};
    unsigned short id;
};
template<typename T>
class AddComponentEvent final : public Event<RegistryEvents> {
    explicit AddComponentEvent(unsigned short id, T component) : Event<RegistryEvents>(RegistryEvents::AddComponent, "AddComponent"), id(id), component(component) {};
    unsigned short id;
    T component;
};
template<typename T>
class RemoveComponentEvent final : public Event<RegistryEvents> {
    explicit RemoveComponentEvent(unsigned short id) : Event<RegistryEvents>(RegistryEvents::RemoveComponent, "RemoveComponent"), id(id){};
    unsigned short id;
};