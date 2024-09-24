#pragma once
#include <memory>

#include "Event.hpp"
#include "CollisionEvents.hpp"
#include "RegistryEvents.hpp"
#include "FactoryEvents.hpp"

class EventHandler {
public:
    EventHandler() : collision_dispatcher(){}
    static EventHandler* GetInstance();
        EventDispatcher<CollisionEvents> collision_dispatcher;
        EventDispatcher<RegistryEvents> registry_dispatcher;
        EventDispatcher<FactoryEvents> factory_dispatcher;

private:
    static std::unique_ptr<EventHandler> s_Instance;
};

