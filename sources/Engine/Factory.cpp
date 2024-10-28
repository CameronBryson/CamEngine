#include "Factory.hpp"
#include "Engine/Event.hpp"
#include "Engine/FactoryEvents.hpp"
#include "Registry.hpp"
factory::factory(registry& m_registry) : m_registry_(m_registry) {
    EventHandler::GetInstance()->factory_dispatcher.AddListener(FactoryEvents::CreateProjectile, [this](const Event<FactoryEvents>& event) {
        this->on_factory_create_projectile_event(event);
    });}
void factory::on_factory_create_projectile_event(const Event<FactoryEvents> &event) {
    auto event_data = event.ToType<CreateProjectileEvent>();
    //printf("Create Projectile Test\n");
    create_projectile(m_registry_, event_data.position, event_data.direction, event_data.speed,event_data.collision_bitmask);
}
unsigned short factory::create_player(registry &registry) {
    const auto id = registry.create_entity();
    registry.add_component<c_player>(id, c_player());
    registry.add_component<c_health>(id, c_health{.health = 100});
    registry.add_component<c_transform>(id, c_transform{
            .position = {0, 5, 10}, .rotation = {0, 3.14, 0}, .scale = {0.2, 0.2, 0.2}
    });
    registry.add_component<c_sphere>(id, c_sphere{.radius = 5.0f});
    //registry.add_component<c_quad>(id, c_quad{.extents = {3.0f, 3.0f, 3.0f}});
    registry.add_component<c_collider>(id, c_collider{.collision_bitmask = settings::player_bitmask});
    registry.add_component<c_model>(id, c_model{.name = "player"});
    registry.add_component<c_dynamic_body>(id,c_dynamic_body{.drag = 0.4f, .angluar_drag = 0.9f});
    registry.add_component<c_damage>(id, c_damage{.damage = 1});
    return id;

}
unsigned short factory::create_sphere(registry &registry, glm::vec3 position, float radius) {
    const auto id = registry.create_entity();
    registry.add_component<c_transform>(id, c_transform{.position = position, .scale = {1.0, 1.0, 1.0}});
    registry.add_component<c_sphere>(id, c_sphere{.radius = radius});
    registry.add_component<c_collider>(id, c_collider{});
    registry.add_component<c_model>(id, c_model{.name = "sphere"});
    return id;
}
unsigned short factory::create_quad(registry &registry, glm::vec3 position, glm::vec3 extents) {
    const auto id = registry.create_entity();
    registry.add_component<c_transform>(id, c_transform{.position = position,.rotation = {0, 0, 0}, .scale = {1.0f,1.0f,1.0f}});
    registry.add_component<c_quad>(id, c_quad{.extents = extents});
    registry.add_component<c_collider>(id, c_collider{ .collision_bitmask = settings::enemy_bitmask});
    registry.add_component<c_model>(id, c_model{.name = "cube"});
    return id;
}
unsigned short factory::create_directional_light(registry &registry, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse,
                                       glm::vec3 specular) {
    const auto id =registry.create_entity();
    registry.add_component<c_directional_light>(id, c_directional_light{.direction = direction, .ambient = ambient, .diffuse = diffuse, .specular = specular} );
    return id;
}
unsigned short factory::create_projectile(registry &registry, glm::vec3 position, glm::vec3 direction, float speed,unsigned int collision_bitmask) {
    const auto id = registry.create_entity();
    registry.add_component<c_transform>(id, c_transform{.position = position});
    registry.add_component<c_damage>(id, c_damage{.damage = 1});
    registry.add_component<c_sphere>(id, c_sphere{.radius = 1});
    registry.add_component<c_health>(id, c_health{.health =  1});
    registry.add_component<c_model>(id, c_model{.name = "sphere"});
    registry.add_component<c_collider>(id, c_collider{.collision_bitmask = collision_bitmask});
    registry.add_component<c_dynamic_body>(id, c_dynamic_body{.drag = 0.0f, .velocity = direction * speed,.angluar_drag = 0.0f});
    //registry.add_component<c_projectile>(id, c_projectile{.direction = direction, .speed = speed});
    return id;
}
unsigned short factory::create_enemy_ship(registry &registry, glm::vec3 position, float radius, glm::vec3 direction,
                                float speed, unsigned short target) {
    const auto id = registry.create_entity();
    registry.add_component<c_transform>(id, c_transform{.position = position,.scale = {0.5f,0.5f,0.5f}});
    registry.add_component<c_health>(id, c_health{.health = 3});
    registry.add_component<c_enemy>(id, c_enemy{.target =  target});
    registry.add_component<c_sphere>(id, c_sphere{.radius = radius});
    registry.add_component<c_model>(id, c_model{.name = "player"});
    registry.add_component<c_collider>(id, c_collider{ .collision_bitmask = settings::enemy_bitmask});
    registry.add_component<c_dynamic_body>(id, c_dynamic_body{.drag = 0.0f, .velocity = direction * speed,.angluar_drag = 0.0f});
    registry.add_component<c_damage>(id, c_damage{.damage = 1});
    return id;
}



