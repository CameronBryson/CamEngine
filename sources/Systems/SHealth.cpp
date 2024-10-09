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

