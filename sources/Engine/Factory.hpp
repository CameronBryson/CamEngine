#pragma once
#include "FactoryEvents.hpp"
#include "Event.hpp"
class BaseScene;
class Factory
{
public:
    explicit Factory(BaseScene* scene);
    ~Factory();
    void onFactoryCreateProjectileEvent(const Event<FactoryEvents>& event);
    void onFactoryCreateEnemyEvent(const Event<FactoryEvents>& event);
    void onFactoryCreateAsteroidEvent(const Event<FactoryEvents>& event);
    void onFactoryCreateSpaceDebrisEvent(const Event<FactoryEvents>& event);
    unsigned short createPlayer();

    unsigned short createSkybox(glm::vec3 position, float radius);
    unsigned short createBoundary(glm::vec3 position, glm::vec3 extents);
    unsigned short createDirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
    unsigned short createPointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic);
    unsigned short createProjectile(glm::vec3 position, glm::vec3 direction, float speed, unsigned int collision_bitmask);
    unsigned short createEnemyShip(glm::vec3 position, float radius, glm::vec3 direction, float speed, unsigned short target);
    unsigned short createAsteroid(glm::vec3 position, float radius, float speed, unsigned short target);
    unsigned short createSpaceDebris(glm::vec3 position, glm::vec3 direction, glm::vec3 spin, float speed);
private:
	BaseScene* m_Scene;
};
