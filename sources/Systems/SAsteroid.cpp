//
// Created by cam on 29/10/24.
//

#include "SAsteroid.hpp"

#include <Engine/Registry.hpp>

void SAsteroid::update(Registry & m_registry, float dt)
{
    auto & asteroids = m_registry.getSparseSet<CAsteroid>();
    auto & transforms = m_registry.getSparseSet<CTransform>();
    auto & dynamic_bodies = m_registry.getSparseSet<CDynamicBody>();
    auto ids = m_registry.getEntityIDs<CAsteroid,CTransform,CDynamicBody>();
    for (auto id : ids)
    {
        auto & asteroid = asteroids.get_item(id);
        auto & transform = transforms.get_item(id);
        auto & dynamic_body = dynamic_bodies.get_item(id);
        auto target_position = transforms.get_item(asteroid.target).position;

        auto direction = glm::normalize(target_position - transform.position) + glm::vec3{0,0,1};
        //dynamic_body.acceleration+=direction*asteroid.speed;
        //dynamic_body.acceleration+=glm::vec3{0,0,-1} * asteroid.speed;


    }

}

