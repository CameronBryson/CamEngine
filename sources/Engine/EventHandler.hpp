#pragma once
#include <memory>

#include "Event.hpp"
#include "CollisionEvents.hpp"
class EventHandler {
public:
    EventHandler() : collision_dispatcher(){}
    static EventHandler* GetInstance();
        EventDispatcher<CollisionEvents> collision_dispatcher;
private:
    static std::unique_ptr<EventHandler> s_Instance;
};

