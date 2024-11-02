//
// Created by cam on 29/10/24.
//

#include "SBoundry.hpp"

#include <Engine/Registry.hpp>

void SBoundary::update(Registry & m_registry, float dt)
{
    auto & transforms = m_registry.getSparseSet<CTransform>();
    auto ids = m_registry.getEntityIDs<CTransform>();
    for (auto id : ids)
    {
        auto & transform = m_registry.getComponent<CTransform>(id);
        if(transform.position.x < settings::world_boundry_min.x)
        {
            m_registry.deleteEntity(id);
        }
        if(transform.position.y < settings::world_boundry_min.y)
        {
            m_registry.deleteEntity(id);
        }
        if(transform.position.z < settings::world_boundry_min.z)
        {
            m_registry.deleteEntity(id);
        }
        if(transform.position.x > settings::world_boundry_max.x)
        {
            m_registry.deleteEntity(id);
        }
        if(transform.position.y > settings::world_boundry_max.y)
        {
            m_registry.deleteEntity(id);
        }
        if(transform.position.z > settings::world_boundry_max.z)
        {
            m_registry.deleteEntity(id);
        }
    }
}
