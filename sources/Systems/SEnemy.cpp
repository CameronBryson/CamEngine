//
// Created by cam on 23/10/24.
//
#include "SEnemy.hpp"

#include <Engine/Registry.hpp>

void s_enemy::update(registry & registry, float dt)
{
    auto & enemies = registry.get_sparse_set<c_enemy>();
    auto & transforms = registry.get_sparse_set<c_transform>();
    auto & dynamic_bodies = registry.get_sparse_set<c_dynamic_body>();
    auto ids = registry.get_entity_ids<c_enemy,c_transform,c_dynamic_body>();
    for (unsigned short id : ids)
    {
        auto & enemy = enemies.get_item(id);
        auto & enemy_tranform = transforms.get_item(id);
        auto & enemy_dynamic_body = dynamic_bodies.get_item(id);
        auto & target_transform = transforms.get_item(enemy.target);
        enemy.time_since_shoot+=dt;
        if(enemy.time_since_shoot>enemy.cooldown)
        {
            //shoot
            //printf("Shoot\n");
            glm::vec3 direction = glm::normalize(target_transform.position-enemy_tranform.position);

            EventHandler::GetInstance()->factory_dispatcher.SendEvent(CreateProjectileEvent(transforms.get_item(id).position, direction, 10.0f,settings::enemy_bitmask));

            enemy.time_since_shoot = 0.0f;
        }
        enemy_dynamic_body.acceleration+=enemy.direction*enemy.speed;
    }

}

