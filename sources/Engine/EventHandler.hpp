#pragma once
#include <memory>

#include "Event.hpp"
#include "CollisionEvents.hpp"
#include "RegistryEvents.hpp"
#include "FactoryEvents.hpp"
#include "HealthEvents.hpp"
#include "InputEvents.hpp"

class EventHandler {
public:
    EventHandler() : collisionDispatcher(){}
    static EventHandler* GetInstance();
        EventDispatcher<CollisionEvents> collisionDispatcher;
        EventDispatcher<RegistryEvents> registryDispatcher;
        EventDispatcher<FactoryEvents> factoryDispatcher;
        EventDispatcher<InputEvents> inputDispatcher;
        EventDispatcher<HealthEvents> healthDispatcher;

private:
    static std::unique_ptr<EventHandler> s_Instance;
};

