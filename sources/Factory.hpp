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
        registry.add_component<c_transform>(id, c_transform());
        registry.add_component<c_velocity>(id, c_velocity());
        registry.add_component<c_rigid_body>(id, c_rigid_body{.drag = 0.9f});
        registry.add_component<c_aabb>(id, c_aabb{.extents = {1.0f, 1.0f, 1.0f}});
        return id;
    }
};