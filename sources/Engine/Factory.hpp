#pragma once
#include "FactoryEvents.hpp"
#include "Event.hpp"
class registry;
class c_transform;
class factory
{
public:
    explicit factory(registry& registry);
    void on_factory_create_projectile_event(const Event<FactoryEvents>& event);
    unsigned short create_player(registry& registry);

    unsigned short create_sphere(registry& registry, glm::vec3 position, float radius);
    unsigned short create_quad(registry& registry, glm::vec3 position, glm::vec3 extents);
    unsigned short create_directional_light(registry& registry, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
    unsigned short create_projectile(registry& registry, glm::vec3 position, glm::vec3 direction, float speed, unsigned int collision_bitmask);
    unsigned short create_enemy_ship(registry& registry, glm::vec3 position, float radius, glm::vec3 direction, float speed, unsigned short target);
private:
    registry& m_registry_;
};
