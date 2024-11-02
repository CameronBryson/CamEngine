

#include "SHealth.hpp"

#include <Engine/Registry.hpp>

void SHealth::update(Registry & registry, float dt)
{
    auto ids = registry.getEntityIDs<CHealth>();
    for( auto id : ids )
    {
        auto & health = registry.getComponent<CHealth>(id);
        if(health.health<=0)
        {
            registry.deleteEntity(id);
        }
    }
}

