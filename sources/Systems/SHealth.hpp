#pragma once
#include "Engine/HealthEvents.hpp"
#include "Engine/Event.hpp"
class Registry;

class SHealth {
public:
    void update(Registry& registry, float dt);
};

