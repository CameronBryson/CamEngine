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
        registry.add_component<c_transform>(id, c_transform{.position = {0,0,0}, .rotation = {0,0,90}, .scale = {1,1,1}});
        registry.add_component<c_velocity>(id, c_velocity());
        registry.add_component<c_kinetic_body>(id, c_kinetic_body());;
        registry.add_component<c_quad>(id, c_quad{.extents = {0.1f, 0.2f, 0.3f}});
        registry.add_component<c_collider>(id, c_collider());
        return id;
    }
};