#pragma once
#include "Event.hpp"
enum class FactoryEvents
{
    CreateProjectile
};
class CreateProjectileEvent final : public Event<FactoryEvents>
{
public:
    explicit CreateProjectileEvent(const glm::vec3 position, const glm::vec3 direction, const float speed) : Event<FactoryEvents>(FactoryEvents::CreateProjectile, "CreateProjectile"),  position(position), direction(direction), speed(speed) {};
    glm::vec3 position;
    glm::vec3 direction;
    float speed;
};