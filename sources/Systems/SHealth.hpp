//
// Created by cam on 08/10/24.
//

#ifndef SHEALTH_HPP
#define SHEALTH_HPP
#include "Engine/HealthEvents.hpp"
#include "Engine/Event.hpp"
class registry;


class s_health {
public:
    static void update(registry& registry, float dt);
    static void on_health_change_event(const Event<HealthEvents>& event);
    static void on_health_set_event(const Event<HealthEvents>& event);

};



#endif //SHEALTH_HPP
