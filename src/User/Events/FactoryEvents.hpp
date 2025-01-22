#pragma once
#include "Engine/Events/Event.hpp"
#include "glm/vec3.hpp"
enum class FactoryEvents : unsigned char
{
    CreateProjectile,
    CreateEnemy,
    CreateAsteroid,
    CreateSpaceDebris

};
class CreateProjectileEvent final : public Event<FactoryEvents>
{
public:
    explicit CreateProjectileEvent(const glm::vec3& position, const glm::vec3& direction, const float speed,unsigned int collision_bitmask) : Event<FactoryEvents>(FactoryEvents::CreateProjectile, "CreateProjectile"),  position(position), direction(direction), collisionBitmask(collision_bitmask), speed(speed) {}
    glm::vec3 position;
    glm::vec3 direction;
    unsigned int collisionBitmask;
    float speed;
};
class CreateEnemyEvent final : public Event<FactoryEvents>
{
public:
    explicit CreateEnemyEvent(const glm::vec3& position, const float radius, const glm::vec3& direction, const float speed, const unsigned short target) : Event<FactoryEvents>(FactoryEvents::CreateEnemy, "CreateEnemy"), position(position), radius(radius), direction(direction), speed(speed), target(target) {}
    glm::vec3 position;
    float radius;
    glm::vec3 direction;
    float speed;
    unsigned short target;
};
class CreateAsteroidEvent final : public Event<FactoryEvents>
{
public:
    explicit CreateAsteroidEvent(const glm::vec3& position, const float radius, const float speed, const unsigned short target) : Event<FactoryEvents>(FactoryEvents::CreateAsteroid, "CreateAsteroid"), position(position), radius(radius), speed(speed),target(target){}
    glm::vec3 position;
    float radius;
    float speed;
    unsigned short target;
};
class CreateSpaceDebrisEvent final : public Event<FactoryEvents>
{
public:
    explicit CreateSpaceDebrisEvent(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& spin, const float speed) : Event<FactoryEvents>(FactoryEvents::CreateSpaceDebris, "CreateSpaceDebris"), position(position), direction(direction), spin(spin), speed(speed) {}
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 spin;
    float speed;
};