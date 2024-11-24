#pragma once
#include "User/Events/FactoryEvents.hpp"
class BaseScene;
class Factory
{
public:
	explicit Factory(BaseScene* scene);
	~Factory();
	void onFactoryCreateProjectileEvent(const Event<FactoryEvents>& event) const;
	void onFactoryCreateEnemyEvent(const Event<FactoryEvents>& event) const;
	void onFactoryCreateAsteroidEvent(const Event<FactoryEvents>& event) const;
	void onFactoryCreateSpaceDebrisEvent(const Event<FactoryEvents>& event) const;
	unsigned short createPlayer() const;

	unsigned short createSkybox(const glm::vec3& position, float radius) const;
	unsigned short createBoundary(const glm::vec3& position, const glm::vec3& extents) const;
	unsigned short createDirectionalLight(const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular) const;
	unsigned short createPointLight(const glm::vec3& position, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float constant, float linear, float quadratic) const;
	unsigned short createProjectile(const glm::vec3& position, const glm::vec3& direction, float speed, unsigned int collisionBitmask) const;
	unsigned short createEnemyShip(const glm::vec3& position, float radius, const glm::vec3& direction, float speed, unsigned short target) const;
	unsigned short createAsteroid(const glm::vec3& position, float radius, float speed, unsigned short target) const;
	unsigned short createSpaceDebris(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& spin, float speed) const;
private:
	BaseScene* m_Scene;
};
