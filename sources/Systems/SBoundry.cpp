//
// Created by cam on 29/10/24.
//

#include "SBoundry.hpp"

#include <Engine/Registry.hpp>

void s_boundry::update(registry & m_registry, float dt)
{
    auto & transforms = m_registry.get_sparse_set<c_transform>();
    auto ids = m_registry.get_entity_ids<c_transform>();
    for (auto id : ids)
    {
        auto & transform = m_registry.get_component<c_transform>(id);
        if(transform.position.x < settings::world_boundry_min.x)
        {
            m_registry.delete_entity(id);
        }
        if(transform.position.y < settings::world_boundry_min.y)
        {
            m_registry.delete_entity(id);
        }
        if(transform.position.z < settings::world_boundry_min.z)
        {
            m_registry.delete_entity(id);
        }
        if(transform.position.x > settings::world_boundry_max.x)
        {
            m_registry.delete_entity(id);
        }
        if(transform.position.y > settings::world_boundry_max.y)
        {
            m_registry.delete_entity(id);
        }
        if(transform.position.z > settings::world_boundry_max.z)
        {
            m_registry.delete_entity(id);
        }
    }
}
