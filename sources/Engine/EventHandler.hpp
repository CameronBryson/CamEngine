#pragma once
#include <memory>

#include "Event.hpp"
#include "CollisionEvents.hpp"
#include "RegistryEvents.hpp"
#include "FactoryEvents.hpp"
#include "HealthEvents.hpp"
#include "InputEvents.hpp"
#include "ScriptEvents.hpp"
#include "ComponentEvents.hpp"

class EventHandler {
public:
    EventHandler() : collisionDispatcher(){}
    static EventHandler* GetInstance();
        EventDispatcher<CollisionEvents> collisionDispatcher;
        EventDispatcher<RegistryEvents> registryDispatcher;
        EventDispatcher<FactoryEvents> factoryDispatcher;
        EventDispatcher<InputEvents> inputDispatcher;
        EventDispatcher<HealthEvents> healthDispatcher;
	    EventDispatcher<ScriptEvents> scriptDispatcher;
		template<typename T>
		static EventDispatcher<ComponentEvents>& GetComponentDispatcher();

private:
    static std::unique_ptr<EventHandler> s_Instance;

};
template<typename T>
EventDispatcher<ComponentEvents>& EventHandler::GetComponentDispatcher()
{
    static EventDispatcher<ComponentEvents> dispatcher;
    return dispatcher;
}
