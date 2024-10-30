#include "Factory.hpp"
#include "Engine/Event.hpp"
#include "Engine/FactoryEvents.hpp"
#include "Registry.hpp"
factory::factory(registry& m_registry) : m_registry_(m_registry) {
    EventHandler::GetInstance()->factory_dispatcher.AddListener(FactoryEvents::CreateProjectile, [this](const Event<FactoryEvents>& event) {
        this->on_factory_create_projectile_event(event);
    });
    EventHandler::GetInstance()->factory_dispatcher.AddListener(FactoryEvents::CreateEnemy, [this](const Event<FactoryEvents>& event) {
        this->on_factory_create_enemy_event(event);
    });
    EventHandler::GetInstance()->factory_dispatcher.AddListener(FactoryEvents::CreateAsteroid, [this](const Event<FactoryEvents>& event) {
        this->on_factory_create_asteroid_event(event);
    });
    EventHandler::GetInstance()->factory_dispatcher.AddListener(FactoryEvents::CreateSpaceDebris, [this](const Event<FactoryEvents>& event) {
        this->on_factory_create_space_debris_event(event);
    });

}

factory::~factory()
{
    //unbind here
}

void factory::on_factory_create_projectile_event(const Event<FactoryEvents> &event) {
    auto event_data = event.ToType<CreateProjectileEvent>();
    //printf("Create Projectile Test\n");
    create_projectile(m_registry_, event_data.position, event_data.direction, event_data.speed,event_data.collision_bitmask);
}

void factory::on_factory_create_enemy_event(const Event<FactoryEvents> & event)
{
    auto event_data = event.ToType<CreateEnemyEvent>();
    create_enemy_ship(m_registry_,event_data.position,event_data.radius,event_data.direction,event_data.speed,event_data.target);
}

void factory::on_factory_create_asteroid_event(const Event<FactoryEvents> & event)
{
    auto event_data = event.ToType<CreateAsteroidEvent>();
    create_asteroid(m_registry_,event_data.position,event_data.radius,event_data.speed,event_data.target);
}

void factory::on_factory_create_space_debris_event(const Event<FactoryEvents> & event)
{
    auto event_data = event.ToType<CreateSpaceDebrisEvent>();
    create_space_debris(m_registry_,event_data.position,event_data.direction,event_data.spin,event_data.speed);
}

unsigned short factory::create_player(registry &registry) {
    const auto id = registry.create_entity();
    registry.add_component<c_player>(id, c_player());
    registry.add_component<c_health>(id, c_health{.health = 100});
    registry.add_component<c_transform>(id, c_transform{
            .position = {0, 0, 0}, .rotation = {0, 3.14, 0}, .scale = {0.1, 0.1, 0.1}
    });
    registry.add_component<c_sphere>(id, c_sphere{.radius = 5.0f});
    //registry.add_component<c_quad>(id, c_quad{.extents = {3.0f, 3.0f, 3.0f}});
    registry.add_component<c_collider>(id, c_collider{.collision_bitmask = settings::player_bitmask});
    registry.add_component<c_model>(id, c_model{.name = "player"});
    registry.add_component<c_dynamic_body>(id,c_dynamic_body{.drag = 0.4f,.elasticity = 0.1f, .angluar_drag = 0.9f});
    registry.add_component<c_damage>(id, c_damage{.damage = 1});
    registry.add_component<c_point_light>(id, c_point_light{.ambient = {0.5,0.5,0.5},.diffuse = {0.9,0.9,0.9},.specular = {0.5,0.5,0.5},.constant = 1,.linear = 0.09,.quadratic = 0.032});

    return id;

}
unsigned short factory::create_skybox(registry &registry, glm::vec3 position, float radius) {
    const auto id = registry.create_entity();
    registry.add_component<c_transform>(id, c_transform{.position = position, .scale = {2.5, 2.5, 2.5}});
    registry.add_component<c_model>(id, c_model{.name = "skybox"});
    registry.add_component<c_background>(id,c_background());
    return id;
}
unsigned short factory::create_boundry(registry &registry, glm::vec3 position, glm::vec3 extents) {
    const auto id = registry.create_entity();
    registry.add_component<c_transform>(id, c_transform{.position = position,.rotation = {0, 0, 0}, .scale = {1.0f,1.0f,1.0f}});
    registry.add_component<c_quad>(id, c_quad{.extents = extents});
    registry.add_component<c_collider>(id, c_collider{ .collision_bitmask = settings::enemy_bitmask});
    return id;
}
unsigned short factory::create_directional_light(registry &registry, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse,
                                       glm::vec3 specular) {
    const auto id =registry.create_entity();
    registry.add_component<c_directional_light>(id, c_directional_light{.direction = direction, .ambient = ambient, .diffuse = diffuse, .specular = specular} );
    return id;
}
unsigned short factory::create_point_light(registry & registry, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic)
{
    const auto id = registry.create_entity();
    registry.add_component<c_transform>(id, c_transform{.position = position});
    registry.add_component<c_point_light>(id, c_point_light{.ambient = ambient,.diffuse = diffuse,.specular = specular,.constant = constant,.linear = linear,.quadratic = quadratic});

    return id;
}

unsigned short factory::create_projectile(registry &registry, glm::vec3 position, glm::vec3 direction, float speed,unsigned int collision_bitmask) {
    const auto id = registry.create_entity();
    registry.add_component<c_transform>(id, c_transform{.position = position, .scale = {0.5,0.5,0.5}});
    registry.add_component<c_damage>(id, c_damage{.damage = 1});
    registry.add_component<c_sphere>(id, c_sphere{.radius = 1});
    registry.add_component<c_health>(id, c_health{.health =  1});
    registry.add_component<c_model>(id, c_model{.name = "sphere"});
    registry.add_component<c_collider>(id, c_collider{.collision_bitmask = collision_bitmask});
    registry.add_component<c_dynamic_body>(id, c_dynamic_body{.drag = 0.0f, .velocity = direction * speed,.angluar_drag = 0.0f});
    registry.add_component<c_point_light>(id, c_point_light{.ambient = {0.4,0.4,0.4},.diffuse = {0.9,0.9,0.9},.specular = {0.8,0.8,0.8},.constant = 1,.linear = 0.14,.quadratic = 0.07});

    //registry.add_component<c_projectile>(id, c_projectile{.direction = direction, .speed = speed});
    return id;
}
unsigned short factory::create_enemy_ship(registry &registry, glm::vec3 position, float radius, glm::vec3 direction,
                                float speed, unsigned short target) {
    const auto id = registry.create_entity();
    registry.add_component<c_transform>(id, c_transform{.position = position,.scale = {0.3f,0.3f,0.3f}});
    registry.add_component<c_health>(id, c_health{.health = 1});
    registry.add_component<c_enemy>(id, c_enemy{.target =  target,.speed = speed,.direction = direction});
    registry.add_component<c_sphere>(id, c_sphere{.radius = radius});
    registry.add_component<c_model>(id, c_model{.name = "player"});
    registry.add_component<c_collider>(id, c_collider{ .collision_bitmask = settings::enemy_bitmask});
    registry.add_component<c_dynamic_body>(id, c_dynamic_body{.drag = 0.8f});
    registry.add_component<c_damage>(id, c_damage{.damage = 1});
    registry.add_component<c_point_light>(id, c_point_light{.ambient = {0.5,0.5,0.5},.diffuse = {1,1,1},.specular = {1,1,1},.constant = 1,.linear = 0.14,.quadratic = 0.07});

    return id;
}
unsigned short factory::create_asteroid(registry & registry, glm::vec3 position, float radius, float speed, unsigned short target)
{
    const auto id = registry.create_entity();
    registry.add_component<c_transform>(id, c_transform{.position =  position,.scale = {1,1,1}});
    registry.add_component<c_asteroid>(id, c_asteroid{.target = target,.speed = speed});
    registry.add_component<c_model>(id, c_model{.name = "asteroid"});
    registry.add_component<c_health>(id, c_health{.health = 3});
    registry.add_component<c_collider>(id, c_collider{.collision_bitmask = settings::enemy_bitmask});
    registry.add_component<c_sphere>(id, c_sphere{.radius = radius});
    registry.add_component<c_dynamic_body>(id, c_dynamic_body{.drag = 0.8f,.angluar_drag = 0.3});
    registry.add_component<c_point_light>(id, c_point_light{.ambient = {0.5,0.5,0.5},.diffuse = {0.9,0.9,0.9},.specular = {0.5,0.5,0.5},.constant = 1,.linear = 0.09,.quadratic = 0.032});
    registry.add_component<c_repeat_acceleration>(id, c_repeat_acceleration{.acceleration = glm::vec3{0,0,1} * speed,.angularAcceleration = {-0.2,-0.2,-0.2}});
    registry.add_component<c_damage>(id,c_damage{.damage = 10});
    return id;

}

unsigned short factory::create_space_debris(registry & registry, glm::vec3 position, glm::vec3 direction, glm::vec3 spin, float speed)
{
    const auto id = registry.create_entity();
    registry.add_component<c_transform>(id, c_transform{.position = position,.scale = {2,2,2}});
    registry.add_component<c_collider>(id, c_collider{.collision_bitmask = settings::enemy_bitmask});
    registry.add_component<c_dynamic_body>(id,c_dynamic_body{.drag = 0.3,.angluar_drag = 0.3});
    registry.add_component<c_quad>(id, c_quad{.extents = {1,1,1}});
    registry.add_component<c_model>(id, c_model{.name = "cube"});
    //registry.add_component<c_point_light>(id, c_point_light{.ambient = {0.5,0.5,0.5},.diffuse = {0.9,0.9,0.9},.specular = {0.5,0.5,0.5},.constant = 1,.linear = 0.09,.quadratic = 0.032});
    registry.add_component<c_repeat_acceleration>(id, c_repeat_acceleration{.acceleration = direction * speed,.angularAcceleration = spin});
    registry.add_component<c_damage>(id,c_damage{.damage = 10});
    return id;
}




