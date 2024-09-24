#pragma once

class registry;

enum class FactoryEvents
{
    CreateProjectile
};
class CreateProjectileEvent final : public Event<FactoryEvents>
{
public:
    explicit CreateProjectileEvent(registry& registry, const glm::vec3 position, const glm::vec3 direction, const float speed) : Event<FactoryEvents>(FactoryEvents::CreateProjectile, "CreateProjectile"), registry_(registry), position(position), direction(direction), speed(speed) {};
    registry& registry_;
    glm::vec3 position;
    glm::vec3 direction;
    float speed;
};