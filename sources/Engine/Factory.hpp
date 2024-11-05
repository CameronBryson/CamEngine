#pragma once
#include "FactoryEvents.hpp"
#include "Event.hpp"
class Registry;
class Factory
{
public:
    explicit Factory(Registry& registry);
    ~Factory();
    void onFactoryCreateProjectileEvent(const Event<FactoryEvents>& event);
    void onFactoryCreateEnemyEvent(const Event<FactoryEvents>& event);
    void onFactoryCreateAsteroidEvent(const Event<FactoryEvents>& event);
    void onFactoryCreateSpaceDebrisEvent(const Event<FactoryEvents>& event);
    unsigned short createPlayer(Registry& registry);

    unsigned short createSkybox(Registry& registry, glm::vec3 position, float radius);
    unsigned short createBoundary(Registry& registry, glm::vec3 position, glm::vec3 extents);
    unsigned short createDirectionalLight(Registry& registry, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
    unsigned short createPointLight(Registry& registry, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic);
    unsigned short createProjectile(Registry& registry, glm::vec3 position, glm::vec3 direction, float speed, unsigned int collision_bitmask);
    unsigned short createEnemyShip(Registry& registry, glm::vec3 position, float radius, glm::vec3 direction, float speed, unsigned short target);
    unsigned short createAsteroid(Registry& registry, glm::vec3 position, float radius, float speed, unsigned short target);
    unsigned short createSpaceDebris(Registry& registry, glm::vec3 position, glm::vec3 direction, glm::vec3 spin, float speed);
private:
    Registry& mRegistry;
};
