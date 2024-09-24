#pragma once
#include "Components.hpp"
#include "Registry.hpp"

class factory
{
public:
    static void bind_events()
    {
        EventHandler::GetInstance()->factory_dispatcher.AddListener(FactoryEvents::CreateProjectile, on_factory_event);
    }
    static void on_factory_event(const Event<FactoryEvents>& event)
    {
        if (event.GetType() == FactoryEvents::CreateProjectile)
        {
            auto event_data = event.ToType<CreateProjectileEvent>();
            printf("Create Projectile Test\n");
            factory::create_projectile(event_data.registry_, event_data.position, event_data.direction, event_data.speed);
        }
    }
    static unsigned short create_player(registry& registry)
    {
        const auto id = registry.create_entity();
        registry.add_component<c_player>(id, c_player());
        registry.add_component<c_transform>(id, c_transform{
                                                .position = {0, 5, 10}, .rotation = {0, 3.14, 0}, .scale = {1.0, 1.0, 1.0}
                                            });
        //registry.add_component<c_sphere>(id, c_sphere{.radius = 2.0f});
        registry.add_component<c_quad>(id, c_quad{.extents = {3.0f, 3.0f, 3.0f}});
        registry.add_component<c_collider>(id, c_collider{.collision_type = object_collision_type::DYNAMIC});
        registry.add_component<c_model>(id, c_model{.name = "player"});
        registry.add_component<c_dynamic_body>(id,c_dynamic_body{.drag = 0.4f, .angluar_drag = 0.8f});
        return id;
    }

    static unsigned short create_sphere(registry& registry, glm::vec3 position, float radius)
    {
        const auto id = registry.create_entity();
        registry.add_component<c_transform>(id, c_transform{.position = position, .scale = {1.0, 1.0, 1.0}});
        registry.add_component<c_sphere>(id, c_sphere{.radius = radius});
        registry.add_component<c_collider>(id, c_collider{.collision_type = object_collision_type::STATIC});
        registry.add_component<c_model>(id, c_model{.name = "sphere"});
        return id;
    }

    static unsigned short create_quad(registry& registry, glm::vec3 position, glm::vec3 extents)
    {
        const auto id = registry.create_entity();
        registry.add_component<c_transform>(id, c_transform{.position = position,.rotation = {54, 13, 127}, .scale = {1.0f,1.0f,1.0f}});
        registry.add_component<c_quad>(id, c_quad{.extents = extents});
        registry.add_component<c_collider>(id, c_collider{.collision_type = object_collision_type::STATIC});
        registry.add_component<c_model>(id, c_model{.name = "cube"});
        return id;
    }
    static unsigned short create_directional_light(registry& registry, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
    {
        const auto id =registry.create_entity();
        registry.add_component<c_directional_light>(id, c_directional_light{.direction = direction, .ambient = ambient, .diffuse = diffuse, .specular = specular} );
        return id;
    }
    static unsigned short create_projectile(registry& registry, glm::vec3 position, glm::vec3 direction, float speed)
    {
        const auto id = registry.create_entity();
        registry.add_component<c_transform>(id, c_transform{.position = position});
        registry.add_component<c_sphere>(id, c_sphere{.radius = 1});
        registry.add_component<c_model>(id, c_model{.name = "sphere"});
        registry.add_component<c_collider>(id, c_collider{.collision_type = object_collision_type::DYNAMIC});
        registry.add_component<c_dynamic_body>(id, c_dynamic_body{.velocity = direction * speed});
        //registry.add_component<c_projectile>(id, c_projectile{.direction = direction, .speed = speed});
        return id;
    }
};
