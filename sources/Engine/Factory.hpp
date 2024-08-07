#pragma once
#include "Components.hpp"
#include "Registry.hpp"

class factory
{
  public:
    static unsigned short create_player(registry &registry)
    {
        const auto id = registry.create_entity();
        registry.add_component<c_player>(id, c_player());
        registry.add_component<c_transform>(id, c_transform{.position = {0,0,0}, .rotation = {0,0,0}, .scale = {1.0,1.0,1.0}});
        registry.add_component<c_velocity>(id, c_velocity());
        registry.add_component<c_quad>(id, c_quad{.extents = {1.0f, 1.0f, 1.0f}});
        registry.add_component<c_collider>(id, c_collider{.collision_type = object_collision_type::DYNAMIC});
        return id;
    }
    static unsigned short create_sphere(registry &registry, glm::vec3 position, float radius)
    {
        const auto id = registry.create_entity();
        registry.add_component<c_transform>(id, c_transform{.position = position, .scale = {1.0,1.0,1.0}});
        registry.add_component<c_sphere>(id, c_sphere{.radius = radius});
        registry.add_component<c_collider>(id, c_collider());
        return id;
    }
    static unsigned short create_quad(registry &registry, glm::vec3 position, glm::vec3 extents)
    {
        const auto id = registry.create_entity();
        registry.add_component<c_transform>(id, c_transform{.position = position, .rotation = {15,76,26}});
        registry.add_component<c_quad>(id, c_quad{.extents = extents});
        registry.add_component<c_collider>(id, c_collider());
        return id;
    }
};