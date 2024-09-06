#pragma once
#include "Components.hpp"
#include "Registry.hpp"

class factory
{
public:
    static unsigned short create_player(registry& registry)
    {
        const auto id = registry.create_entity();
        registry.add_component<c_player>(id, c_player());
        registry.add_component<c_transform>(id, c_transform{
                                                .position = {0, 0, 0}, .rotation = {65, 86, 13}, .scale = {1.0, 1.0, 1.0}
                                            });
        registry.add_component<c_velocity>(id, c_velocity());
        registry.add_component<c_quad>(id, c_quad{.extents = {1.0f, 1.0f, 1.0f}});
        registry.add_component<c_collider>(id, c_collider{.collision_type = object_collision_type::DYNAMIC});
        registry.add_component<c_model>(id, c_model{.name = "player"});
        return id;
    }

    static unsigned short create_sphere(registry& registry, glm::vec3 position, float radius)
    {
        const auto id = registry.create_entity();
        registry.add_component<c_transform>(id, c_transform{.position = position, .scale = {1.0, 1.0, 1.0}});
        registry.add_component<c_sphere>(id, c_sphere{.radius = radius});
        registry.add_component<c_collider>(id, c_collider());
        registry.add_component<c_model>(id, c_model{.name = "sphere"});
        return id;
    }

    static unsigned short create_quad(registry& registry, glm::vec3 position, glm::vec3 extents)
    {
        const auto id = registry.create_entity();
        registry.add_component<c_transform>(id, c_transform{.position = position,.rotation = {280, 83, 76}, .scale = {1.0f,1.0f,1.0f}});
        registry.add_component<c_quad>(id, c_quad{.extents = extents});
        registry.add_component<c_collider>(id, c_collider());
        registry.add_component<c_model>(id, c_model{.name = "cube"});
        return id;
    }
    static unsigned short create_directional_light(registry& registry, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
    {
        const auto id =registry.create_entity();
        registry.add_component<c_directional_light>(id, c_directional_light{.direction = direction, .ambient = ambient, .diffuse = diffuse, .specular = specular} );
        return id;
    }
};
