#pragma once
#include "Event.hpp"
enum class HealthEvents
{
    HealthChange,
    HealthSet
};
class HealthChangeEvent final : public Event<HealthEvents>
{
public:
    explicit HealthChangeEvent(unsigned short id, float change) : Event<HealthEvents>(HealthEvents::HealthChange, "HealthChange"), id(id), change(change) {}
    unsigned short id;
    float change;

};
class HealthSetEvent final : public Event<HealthEvents>
{
public:
    explicit HealthSetEvent(unsigned short id, float health) : Event<HealthEvents>(HealthEvents::HealthSet, "HealthSet"), id(id), health(health) {}
    unsigned short id;
    float health;
};