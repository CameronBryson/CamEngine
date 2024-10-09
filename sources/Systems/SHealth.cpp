//
// Created by cam on 08/10/24.
//

#include "SHealth.hpp"

#include <Engine/Registry.hpp>

void s_health::update(registry & registry, float dt)
{
    auto ids = registry.get_entity_ids<c_health>();
    for( auto id : ids )
    {
        auto & health = registry.get_component<c_health>(id);
        if(health.health<=0)
        {
            registry.delete_entity(id);
        }
    }
}
void s_health::on_health_change_event(const Event<HealthEvents> & event)
{
    // const auto event_data = event.ToType<HealthChangeEvent>();
    // if(!registry.has_component<c_health>(event_data.id)) return;
    // auto & health = registry.get_component<c_health>(event_data.id);
    // health.health-=event_data.change;

}
void s_health::on_health_set_event(const Event<HealthEvents> & event)
{
    // const auto event_data = event.ToType<HealthSetEvent>();
    // if(!registry.has_component<c_health>(event_data.id)) return;
    // auto & health = registry.get_component<c_health>(event_data.id);
    // health.health=event_data.health;
}


