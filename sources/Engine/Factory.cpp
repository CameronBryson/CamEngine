#include "Factory.hpp"
#include "Engine/Event.hpp"
#include "Engine/FactoryEvents.hpp"
#include "BaseScene.hpp"
#include "Scripts/Health.hpp"
#include "Scripts/Player.hpp"
#include "Scripts/EnemyShip.hpp"
#include "Scripts/Asteroid.hpp"
#include "Scripts/Damage.hpp"

Factory::Factory(BaseScene* scene) : m_Scene(scene) {
	EventHandler::GetInstance()->factoryDispatcher.AddListener(FactoryEvents::CreateProjectile, [this](const Event<FactoryEvents>& event) {
		this->onFactoryCreateProjectileEvent(event);
	});
	EventHandler::GetInstance()->factoryDispatcher.AddListener(FactoryEvents::CreateEnemy, [this](const Event<FactoryEvents>& event) {
		this->onFactoryCreateEnemyEvent(event);
	});
	EventHandler::GetInstance()->factoryDispatcher.AddListener(FactoryEvents::CreateAsteroid, [this](const Event<FactoryEvents>& event) {
		this->onFactoryCreateAsteroidEvent(event);
	});
	EventHandler::GetInstance()->factoryDispatcher.AddListener(FactoryEvents::CreateSpaceDebris, [this](const Event<FactoryEvents>& event) {
		this->onFactoryCreateSpaceDebrisEvent(event);
	});

}

Factory::~Factory()
{
	//unbind here
}

void Factory::onFactoryCreateProjectileEvent(const Event<FactoryEvents> &event) {
	auto event_data = event.ToType<CreateProjectileEvent>();
	//printf("Create Projectile Test\n");

	createProjectile(event_data.position, event_data.direction, event_data.speed,event_data.collisionBitmask);
}

void Factory::onFactoryCreateEnemyEvent(const Event<FactoryEvents> & event)
{
	auto event_data = event.ToType<CreateEnemyEvent>();

	createEnemyShip(event_data.position,event_data.radius,event_data.direction,event_data.speed,event_data.target);
}

void Factory::onFactoryCreateAsteroidEvent(const Event<FactoryEvents> & event)
{
	auto event_data = event.ToType<CreateAsteroidEvent>();

	createAsteroid(event_data.position,event_data.radius,event_data.speed,event_data.target);
}

void Factory::onFactoryCreateSpaceDebrisEvent(const Event<FactoryEvents> & event)
{
	auto event_data = event.ToType<CreateSpaceDebrisEvent>();

	createSpaceDebris(event_data.position,event_data.direction,event_data.spin,event_data.speed);
}

unsigned short Factory::createPlayer() {
	const auto id = m_Scene->createEntity();
	m_Scene->addComponent<Player>(id);
	m_Scene->addComponent<Health>(id, 100);
	m_Scene->addComponent<CTransform>(id, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 3.14, 0 }, glm::vec3{ 0.1, 0.1, 0.1 });
	m_Scene->addComponent<CSphereBounds>(id, 5.0f);
	//registry.add_component<c_quad>(id, c_quad{.extents = {3.0f, 3.0f, 3.0f}});
	m_Scene->addComponent<CCollider>(id, true,settings::player_bitmask);
	m_Scene->addComponent<CModel>(id, "player");
	m_Scene->addComponent<CDynamicBody>(id, 0.1f, 0.4f, 0.9f);
	m_Scene->addComponent<Damage>(id, 1);
	m_Scene->addComponent<CPointLight>(id, glm::vec3{ 0.8, 0.8, 0.8 }, glm::vec3{ 0.9, 0.9, 0.9 }, glm::vec3{ 0.5, 0.5, 0.5 }, 1.0f, 0.09f, 0.032f);

	return id;

}
unsigned short Factory::createSkybox(glm::vec3 position, float radius) {
	const auto id = m_Scene->createEntity();
	m_Scene->addComponent<CTransform>(id, position, glm::vec3{ 0, 0, 0 }, glm::vec3{ 5, 5, 5 });
	m_Scene->addComponent<CModel>(id, "skybox");
	m_Scene->addComponent<CBackground>(id);
	return id;
}
unsigned short Factory::createBoundary(glm::vec3 position, glm::vec3 extents) {
	const auto id = m_Scene->createEntity();
	m_Scene->addComponent<CTransform>(id, position,glm::vec3{0, 0, 0}, glm::vec3{1.0f,1.0f,1.0f});
	m_Scene->addComponent<CBoxBounds>(id, extents);
	m_Scene->addComponent<CCollider>(id, false,settings::enemy_bitmask);
	return id;
}
unsigned short Factory::createDirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse,
									   glm::vec3 specular) {
	const auto id = m_Scene->createEntity();
	m_Scene->addComponent<CDirectionalLight>(id, direction, ambient, diffuse, specular);
	return id;
}
unsigned short Factory::createPointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic)
{
	const auto id = m_Scene->createEntity();
	m_Scene->addComponent<CTransform>(id, position, glm::vec3{0,0,0}, glm::vec3{0,0,0});
	m_Scene->addComponent<CPointLight>(id, ambient, diffuse, specular, constant, linear, quadratic);

	return id;
}

unsigned short Factory::createProjectile(glm::vec3 position, glm::vec3 direction, float speed,unsigned int collision_bitmask) {
	const auto id = m_Scene->createEntity();
	m_Scene->addComponent<CTransform>(id, position, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0.5, 0.5, 0.5 });
	m_Scene->addComponent<Damage>(id, 1);
	m_Scene->addComponent<CSphereBounds>(id, 1);
	m_Scene->addComponent<Health>(id, 1);
	m_Scene->addComponent<CModel>(id, "sphere");
	m_Scene->addComponent<CCollider>(id,false,collision_bitmask);
	m_Scene->addComponent<CDynamicBody>(id, 0,0,0);
	m_Scene->addComponent<CPointLight>(id, glm::vec3{ 0.4, 0.4, 0.4 }, glm::vec3{ 0.9, 0.9, 0.9 }, glm::vec3{ 0.8, 0.8, 0.8 }, 1.0f, 0.14f, 0.07f);

	// m_Scene->add_component<c_projectile>(id, c_projectile{.direction = direction, .speed = speed});
	return id;
}
unsigned short Factory::createEnemyShip(glm::vec3 position, float radius, glm::vec3 direction,
								float speed, unsigned short target) {
	const auto id = m_Scene->createEntity();
	m_Scene->addComponent<CTransform>(id, position, glm::vec3{ 0, -3.14f / 2, 0 }, glm::vec3{ 0.5f, 0.5f, 0.5f });
	m_Scene->addComponent<Health>(id, 1);
	m_Scene->addComponent<EnemyShip>(id, target, speed, direction);
	m_Scene->addComponent<CBoxBounds>(id, glm::vec3{ 8, 2, 8 });
	m_Scene->addComponent<CModel>(id, "enemy");
	m_Scene->addComponent<CCollider>(id, false, settings::enemy_bitmask);
	m_Scene->addComponent<CDynamicBody>(id, 0.5,0.8,0.5);
	m_Scene->addComponent<Damage>(id, 1);
	m_Scene->addComponent<CPointLight>(id, glm::vec3{ 0.5, 0.5, 0.5 }, glm::vec3{ 1, 1, 1 }, glm::vec3{ 1, 1, 1 }, 1.0f, 0.14f, 0.07f);

	return id;
}
unsigned short Factory::createAsteroid(glm::vec3 position, float radius, float speed, unsigned short target)
{
	const auto id = m_Scene->createEntity();
	m_Scene->addComponent<CTransform>(id, position, glm::vec3{ 0, 0, 0 }, glm::vec3{ 1, 1, 1 });
	m_Scene->addComponent<Asteroid>(id, target, speed);
	m_Scene->addComponent<CModel>(id, "asteroid");
	m_Scene->addComponent<Health>(id, 3);
	m_Scene->addComponent<CCollider>(id, false, settings::enemy_bitmask);
	m_Scene->addComponent<CSphereBounds>(id, radius);
	m_Scene->addComponent<CDynamicBody>(id,0.5, 0.8f, 0.3f);
	m_Scene->addComponent<CPointLight>(id, glm::vec3{ 0.5, 0.5, 0.5 }, glm::vec3{ 0.9, 0.9, 0.9 }, glm::vec3{ 0.5, 0.5, 0.5 }, 1.0f, 0.09f, 0.032f);
	m_Scene->addComponent<CRepeatAcceleration>(id, glm::vec3{ 0, 0, 1 } * speed, glm::vec3{ -0.2, -0.2, -0.2 });
	m_Scene->addComponent<Damage>(id, 10);
	return id;

}

unsigned short Factory::createSpaceDebris(glm::vec3 position, glm::vec3 direction, glm::vec3 spin, float speed)
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




