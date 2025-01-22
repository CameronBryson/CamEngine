#pragma once
#include "Event.hpp"

enum class ScriptEvents
{
    Init,
    LateInit,
    Update,
    LateUpdate,
    Render,
    Shutdown
};

class InitEvent final : public Event<ScriptEvents>
{
  public:
    explicit InitEvent() : Event<ScriptEvents>(ScriptEvents::Init, "Init")
    {
    }
};
class LateInitEvent final : public Event<ScriptEvents>
{
  public:
    explicit LateInitEvent() : Event<ScriptEvents>(ScriptEvents::LateInit, "LateInit")
    {
    }
};
class UpdateEvent final : public Event<ScriptEvents>
{
  public:
    explicit UpdateEvent(float deltaTime) : Event<ScriptEvents>(ScriptEvents::Update, "Update"), deltaTime(deltaTime)
    {
    }
    float deltaTime;
};
class LateUpdateEvent final : public Event<ScriptEvents>
{
  public:
    explicit LateUpdateEvent(float deltaTime) : Event<ScriptEvents>(ScriptEvents::LateUpdate, "LateUpdate"), deltaTime(deltaTime)
    {
    }
    float deltaTime;
};
class RenderEvent final : public Event<ScriptEvents>
{
  public:
    explicit RenderEvent() : Event<ScriptEvents>(ScriptEvents::Render, "Render")
    {
    }
};
class ShutdownEvent final : public Event<ScriptEvents>
{
  public:
    explicit ShutdownEvent() : Event<ScriptEvents>(ScriptEvents::Shutdown, "Shutdown")
    {
    }
};