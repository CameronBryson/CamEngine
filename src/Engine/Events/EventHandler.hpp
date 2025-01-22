#pragma once
#include <memory>

#include "Event.hpp"
#include "InputEvents.hpp"
#include "ScriptEvents.hpp"
#include "ComponentEvents.hpp"
#include "CommandEvents.hpp"
#include "User/Events/FactoryEvents.hpp"
#include <User/Events/HealthEvents.hpp>

class EventHandler
{
public:
    EventHandler() {}
    ~EventHandler()
    {

    }
    static EventHandler* GetInstance();
    EventDispatcher<FactoryEvents> factoryDispatcher;
    EventDispatcher<InputEvents> inputDispatcher;
    EventDispatcher<HealthEvents> healthDispatcher;
    EventDispatcher<ScriptEvents> scriptDispatcher;
    EventDispatcher<CommandEvents> commandDispatcher;
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
