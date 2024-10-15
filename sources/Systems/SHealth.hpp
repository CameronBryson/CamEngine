#pragma once
#include "Engine/HealthEvents.hpp"
#include "Engine/Event.hpp"
class registry;

class s_health {
public:
    void update(registry& registry, float dt);
};

