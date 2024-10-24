#pragma once
#include "Event.hpp"
#include "glm/glm.hpp"
enum class FactoryEvents
{
    CreateProjectile
};
class CreateProjectileEvent final : public Event<FactoryEvents>
{
public:
    explicit CreateProjectileEvent(const glm::vec3 position, const glm::vec3 direction, const float speed,unsigned int collision_bitmask) : Event<FactoryEvents>(FactoryEvents::CreateProjectile, "CreateProjectile"),  position(position), direction(direction), speed(speed), collision_bitmask(collision_bitmask) {};
    glm::vec3 position;
    glm::vec3 direction;
    unsigned int collision_bitmask;
    float speed;
};