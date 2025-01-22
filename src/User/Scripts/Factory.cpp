#include "pch.hpp"
#include "Factory.hpp"
#include "Engine/Events/Event.hpp"
#include "User/Events/FactoryEvents.hpp"
#include "Engine/Base/BaseScene.hpp"
#include "User/Scripts/Components/Health.hpp"
#include "User/Scripts/Components/Player.hpp"
#include "User/Scripts/Components/EnemyShip.hpp"
#include "User/Scripts/Components/Asteroid.hpp"
#include "User/Scripts/Components/Damage.hpp"

Factory::Factory(BaseScene* scene) : m_Scene(scene)
{
	auto* eventHandler = EventHandler::GetInstance();

	// Bind factory events and store handles
	mFactoryEventHandles.push_back(eventHandler->factoryDispatcher.AddListener(
	    FactoryEvents::CreateProjectile,
	    [this](const Event<FactoryEvents>& event) { onFactoryCreateProjectileEvent(event); }));

	mFactoryEventHandles.push_back(eventHandler->factoryDispatcher.AddListener(
	    FactoryEvents::CreateEnemy, [this](const Event<FactoryEvents>& event) { onFactoryCreateEnemyEvent(event); }));

	mFactoryEventHandles.push_back(eventHandler->factoryDispatcher.AddListener(
	    FactoryEvents::CreateAsteroid,
	    [this](const Event<FactoryEvents>& event) { onFactoryCreateAsteroidEvent(event); }));

	mFactoryEventHandles.push_back(eventHandler->factoryDispatcher.AddListener(
	    FactoryEvents::CreateSpaceDebris,
	    [this](const Event<FactoryEvents>& event) { onFactoryCreateSpaceDebrisEvent(event); }));
}

Factory::~Factory()
{
	auto* eventHandler = EventHandler::GetInstance();

	// Unbind factory events using stored handles
	for (const auto& handle : mFactoryEventHandles)
	{
		eventHandler->factoryDispatcher.RemoveListener(handle);
	}
	mFactoryEventHandles.clear();
}

void Factory::onFactoryCreateProjectileEvent(const Event<FactoryEvents> &event) const
{
	const auto& eventData = event.ToType<CreateProjectileEvent>();
	//printf("Create Projectile Test\n");

	createProjectile(eventData.position, eventData.direction, eventData.speed,eventData.collisionBitmask);
}

void Factory::onFactoryCreateEnemyEvent(const Event<FactoryEvents> & event) const
{
	const auto& eventData = event.ToType<CreateEnemyEvent>();

	createEnemyShip(eventData.position,eventData.radius,eventData.direction,eventData.speed,eventData.target);
}

void Factory::onFactoryCreateAsteroidEvent(const Event<FactoryEvents> & event) const
{
	const auto& eventData = event.ToType<CreateAsteroidEvent>();

	createAsteroid(eventData.position,eventData.radius,eventData.speed,eventData.target);
}

void Factory::onFactoryCreateSpaceDebrisEvent(const Event<FactoryEvents> & event) const
{
	const auto& eventData = event.ToType<CreateSpaceDebrisEvent>();

	createSpaceDebris(eventData.position,eventData.direction,eventData.spin,eventData.speed);
}

unsigned short Factory::createPlayer() const
{
	const auto id = m_Scene->createEntity();
	m_Scene->addComponent<Player>(id);
	m_Scene->addComponent<Health>(id, 100);
	m_Scene->addComponent<CTransform>(id, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 3.14, 0 }, glm::vec3{ 0.1, 0.1, 0.1 });
	m_Scene->addComponent<CSphereBounds>(id, 5.0f);
	//registry.add_component<c_quad>(id, c_quad{.extents = {3.0f, 3.0f, 3.0f}});
	m_Scene->addComponent<CCollider>(id, true,settings::player_bitmask);
	m_Scene->addComponent<CModel>(id, "player");
	m_Scene->addComponent<CDynamicBody>(id, 0.1f, 0.7f, 0.9f);
	m_Scene->addComponent<Damage>(id, 1);
	m_Scene->addComponent<CPointLight>(id, glm::vec3{ 0.0, 0.0, 0.0 }, glm::vec3{ 0.4, 0.4, 0.4 }, glm::vec3{ 0.2, 0.2, 0.2 }, 1.0f, 0.09f, 0.032f);

	return id;

}
unsigned short Factory::createSkybox(const glm::vec3& position, float radius) const
{
	const auto id = m_Scene->createEntity();
	m_Scene->addComponent<CTransform>(id, position, glm::vec3{ 0, 0, 0 }, glm::vec3{ 5, 5, 5 });
	m_Scene->addComponent<CModel>(id, "skybox");
	m_Scene->addComponent<CBackground>(id);
	return id;
}
unsigned short Factory::createBoundary(const glm::vec3& position, const glm::vec3& extents) const
{
	const auto id = m_Scene->createEntity();
	m_Scene->addComponent<CTransform>(id, position,glm::vec3{0, 0, 0}, glm::vec3{1.0f,1.0f,1.0f});
	m_Scene->addComponent<CBoxBounds>(id, extents);
	m_Scene->addComponent<CCollider>(id, false,settings::enemy_bitmask);
	return id;
}
unsigned short Factory::createDirectionalLight(const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse,
									   const glm::vec3& specular) const
{
	const auto id = m_Scene->createEntity();
	m_Scene->addComponent<CDirectionalLight>(id, direction, ambient, diffuse, specular);
	return id;
}
unsigned short Factory::createPointLight(const glm::vec3& position, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float constant, float linear, float quadratic) const
{
	const auto id = m_Scene->createEntity();
	m_Scene->addComponent<CTransform>(id, position, glm::vec3{0,0,0}, glm::vec3{0,0,0});
	m_Scene->addComponent<CPointLight>(id, ambient, diffuse, specular, constant, linear, quadratic);

	return id;
}

unsigned short Factory::createProjectile(const glm::vec3& position, const glm::vec3& direction, float speed,unsigned int collisionBitmask) const
{
	const auto id = m_Scene->createEntity();
	m_Scene->addComponent<CTransform>(id, position, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0.5, 0.5, 0.5 });
	m_Scene->addComponent<Damage>(id, 1);
	m_Scene->addComponent<CSphereBounds>(id, 1);
	m_Scene->addComponent<Health>(id, 1);
	m_Scene->addComponent<CModel>(id, "sphere");
	m_Scene->addComponent<CCollider>(id,false,collisionBitmask);
	m_Scene->addComponent<CDynamicBody>(id, 0,0,0);
	m_Scene->addComponent<CPointLight>(id, glm::vec3{ 0.0, 0.0, 0.0 }, glm::vec3{ 0.2, 0.2, 0.2 }, glm::vec3{ 0.3, 0.3, 0.3 }, 1.0f, 0.14f, 0.07f);
	m_Scene->addComponent<CRepeatAcceleration>(id, direction * speed, glm::vec3{0, 0, 0});

	// m_Scene->add_component<c_projectile>(id, c_projectile{.direction = direction, .speed = speed});
	return id;
}
unsigned short Factory::createEnemyShip(const glm::vec3& position, float radius, const glm::vec3& direction,
								float speed, unsigned short target) const
{
	const auto id = m_Scene->createEntity();
	m_Scene->addComponent<CTransform>(id, position, glm::vec3{ 0, -3.14f / 2, 0 }, glm::vec3{ 0.5f, 0.5f, 0.5f });
	m_Scene->addComponent<Health>(id, 1);
	m_Scene->addComponent<EnemyShip>(id, target, speed, direction);
	m_Scene->addComponent<CBoxBounds>(id, glm::vec3{ 8, 2, 8 });
	m_Scene->addComponent<CModel>(id, "enemy");
	m_Scene->addComponent<CCollider>(id, false, settings::enemy_bitmask);
	m_Scene->addComponent<CDynamicBody>(id, 0.5,0.8,0.5);
	m_Scene->addComponent<Damage>(id, 1);
	m_Scene->addComponent<CPointLight>(id, glm::vec3{ 0.0, 0.0, 0.0 }, glm::vec3{ 0.3, 0.3, 0.3 }, glm::vec3{ 0.2, 0.2, 0.2 }, 1.0f, 0.14f, 0.07f);

	return id;
}
unsigned short Factory::createAsteroid(const glm::vec3& position, float radius, float speed, unsigned short target) const
{
	const auto id = m_Scene->createEntity();
	m_Scene->addComponent<CTransform>(id, position, glm::vec3{ 0, 0, 0 }, glm::vec3{ 1, 1, 1 });
	m_Scene->addComponent<Asteroid>(id, target, speed);
	m_Scene->addComponent<CModel>(id, "asteroid");
	m_Scene->addComponent<Health>(id, 3);
	m_Scene->addComponent<CCollider>(id, false, settings::enemy_bitmask);
	m_Scene->addComponent<CSphereBounds>(id, radius);
	m_Scene->addComponent<CDynamicBody>(id,0.5, 0.8f, 0.3f);
	m_Scene->addComponent<CPointLight>(id, glm::vec3{ 0.0, 0.0, 0.0 }, glm::vec3{ 0.3, 0.3, 0.3 }, glm::vec3{ 0.3, 0.3, 0.3 }, 1.0f, 0.09f, 0.032f);
	m_Scene->addComponent<CRepeatAcceleration>(id, glm::vec3{ 0, 0, 1 } * speed, glm::vec3{ -0.2, -0.2, -0.2 });
	m_Scene->addComponent<Damage>(id, 10);
	return id;

}

unsigned short Factory::createSpaceDebris(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& spin, float speed) const
{
	const auto id = m_Scene->createEntity();
	m_Scene->addComponent<CTransform>(id, position, glm::vec3{ 0, 0, 0 }, glm::vec3{ 2, 2, 2 });
	m_Scene->addComponent<CCollider>(id, false, settings::enemy_bitmask);
	m_Scene->addComponent<CDynamicBody>(id,0.5, 0.3f, 0.3f);
	m_Scene->addComponent<CBoxBounds>(id, glm::vec3{ 5, 1, 2.75 });
	m_Scene->addComponent<CModel>(id, "sat");
	m_Scene->addComponent<CRepeatAcceleration>(id, direction * speed, spin);
	m_Scene->addComponent<Damage>(id, 10);
	return id;
}




