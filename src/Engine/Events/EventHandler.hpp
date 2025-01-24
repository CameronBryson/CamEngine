#pragma once
#include <memory>

#include "Event.hpp"
#include "InputEvents.hpp"
#include "ScriptEvents.hpp"
#include "ComponentEvents.hpp"
#include "CommandEvents.hpp"

class EventHandler
{
public:
    EventHandler() {}
    ~EventHandler()
    {

    }
    static EventHandler* GetInstance();
    EventDispatcher<InputEvents> inputDispatcher;
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
