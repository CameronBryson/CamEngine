//
// Created by cam on 29/10/24.
//

#include "SAsteroid.hpp"

#include <Engine/Registry.hpp>

void s_asteroid::update(registry & m_registry, float dt)
{
    auto & asteroids = m_registry.get_sparse_set<c_asteroid>();
    auto & transforms = m_registry.get_sparse_set<c_transform>();
    auto & dynamic_bodies = m_registry.get_sparse_set<c_dynamic_body>();
    auto ids = m_registry.get_entity_ids<c_asteroid,c_transform,c_dynamic_body>();
    for (auto id : ids)
    {
        auto & asteroid = asteroids.get_item(id);
        auto & transform = transforms.get_item(id);
        auto & dynamic_body = dynamic_bodies.get_item(id);
        auto target_position = transforms.get_item(asteroid.target).position;

        auto direction = glm::normalize(target_position - transform.position);
        dynamic_body.acceleration+=direction*asteroid.speed;


    }

}

