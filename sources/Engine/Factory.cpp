#include "Factory.hpp"
#include "Engine/Event.hpp"
#include "Engine/FactoryEvents.hpp"
#include "Registry.hpp"
#include "Scripts/Health.hpp"
#include "Scripts/Player.hpp"
#include "Scripts/EnemyShip.hpp"
#include "Scripts/Asteroid.hpp"
#include "Scripts/Damage.hpp"
Factory::Factory(Registry& m_registry) : mRegistry(m_registry) {
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

    createProjectile(mRegistry, event_data.position, event_data.direction, event_data.speed,event_data.collision_bitmask);
}

void Factory::onFactoryCreateEnemyEvent(const Event<FactoryEvents> & event)
{
    auto event_data = event.ToType<CreateEnemyEvent>();

    createEnemyShip(mRegistry,event_data.position,event_data.radius,event_data.direction,event_data.speed,event_data.target);
}

void Factory::onFactoryCreateAsteroidEvent(const Event<FactoryEvents> & event)
{
    auto event_data = event.ToType<CreateAsteroidEvent>();

    createAsteroid(mRegistry,event_data.position,event_data.radius,event_data.speed,event_data.target);
}

void Factory::onFactoryCreateSpaceDebrisEvent(const Event<FactoryEvents> & event)
{
    auto event_data = event.ToType<CreateSpaceDebrisEvent>();

    createSpaceDebris(mRegistry,event_data.position,event_data.direction,event_data.spin,event_data.speed);
}

unsigned short Factory::createPlayer(Registry &registry) {
    const auto id = registry.createEntity();
    registry.addComponent<Player>(id);
    registry.addComponent<Health>(id, 100);
    registry.addComponent<CTransform>(id, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 3.14, 0 }, glm::vec3{ 0.1, 0.1, 0.1 });
    registry.addComponent<CSphereBounds>(id, 5.0f);
    //registry.add_component<c_quad>(id, c_quad{.extents = {3.0f, 3.0f, 3.0f}});
    registry.addComponent<CCollider>(id, true,settings::player_bitmask);
    registry.addComponent<CModel>(id, "player");
    registry.addComponent<CDynamicBody>(id, 0.1f, 0.4f, 0.9f);
    registry.addComponent<Damage>(id, 1);
    registry.addComponent<CPointLight>(id, glm::vec3{ 0.8, 0.8, 0.8 }, glm::vec3{ 0.9, 0.9, 0.9 }, glm::vec3{ 0.5, 0.5, 0.5 }, 1.0f, 0.09f, 0.032f);

    return id;

}
unsigned short Factory::createSkybox(Registry &registry, glm::vec3 position, float radius) {
    const auto id = registry.createEntity();
    registry.addComponent<CTransform>(id, position, glm::vec3{ 0, 0, 0 }, glm::vec3{ 5, 5, 5 });
    registry.addComponent<CModel>(id, "skybox");
    registry.addComponent<CBackground>(id);
    return id;
}
unsigned short Factory::createBoundary(Registry &registry, glm::vec3 position, glm::vec3 extents) {
    const auto id = registry.createEntity();
    registry.addComponent<CTransform>(id, position,glm::vec3{0, 0, 0}, glm::vec3{1.0f,1.0f,1.0f});
    registry.addComponent<CBoxBounds>(id, extents);
    registry.addComponent<CCollider>(id, false,settings::enemy_bitmask);
    return id;
}
unsigned short Factory::createDirectionalLight(Registry &registry, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse,
                                       glm::vec3 specular) {
    const auto id =registry.createEntity();
    registry.addComponent<CDirectionalLight>(id, direction, ambient, diffuse, specular);
    return id;
}
unsigned short Factory::createPointLight(Registry & registry, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic)
{
    const auto id = registry.createEntity();
    registry.addComponent<CTransform>(id, position, glm::vec3{0,0,0}, glm::vec3{0,0,0});
    registry.addComponent<CPointLight>(id, ambient, diffuse, specular, constant, linear, quadratic);

    return id;
}

unsigned short Factory::createProjectile(Registry &registry, glm::vec3 position, glm::vec3 direction, float speed,unsigned int collision_bitmask) {
    const auto id = registry.createEntity();
    registry.addComponent<CTransform>(id, position, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0.5, 0.5, 0.5 });
    registry.addComponent<Damage>(id, 1);
    registry.addComponent<CSphereBounds>(id, 1);
    registry.addComponent<Health>(id, 1);
    registry.addComponent<CModel>(id, "sphere");
    registry.addComponent<CCollider>(id,false,collision_bitmask);
    registry.addComponent<CDynamicBody>(id, 0,0,0);
    registry.addComponent<CPointLight>(id, glm::vec3{ 0.4, 0.4, 0.4 }, glm::vec3{ 0.9, 0.9, 0.9 }, glm::vec3{ 0.8, 0.8, 0.8 }, 1.0f, 0.14f, 0.07f);

    //registry.add_component<c_projectile>(id, c_projectile{.direction = direction, .speed = speed});
    return id;
}
unsigned short Factory::createEnemyShip(Registry &registry, glm::vec3 position, float radius, glm::vec3 direction,
                                float speed, unsigned short target) {
    const auto id = registry.createEntity();
    registry.addComponent<CTransform>(id, position, glm::vec3{ 0, -3.14f / 2, 0 }, glm::vec3{ 0.5f, 0.5f, 0.5f });
    registry.addComponent<Health>(id, 1);
    registry.addComponent<EnemyShip>(id, target, speed, direction);
    registry.addComponent<CBoxBounds>(id, glm::vec3{ 8, 2, 8 });
    registry.addComponent<CModel>(id, "enemy");
    registry.addComponent<CCollider>(id, false, settings::enemy_bitmask);
    registry.addComponent<CDynamicBody>(id, 0.5,0.8,0.5);
    registry.addComponent<Damage>(id, 1);
    registry.addComponent<CPointLight>(id, glm::vec3{ 0.5, 0.5, 0.5 }, glm::vec3{ 1, 1, 1 }, glm::vec3{ 1, 1, 1 }, 1.0f, 0.14f, 0.07f);

    return id;
}
unsigned short Factory::createAsteroid(Registry & registry, glm::vec3 position, float radius, float speed, unsigned short target)
{
    const auto id = registry.createEntity();
    registry.addComponent<CTransform>(id, position, glm::vec3{ 0, 0, 0 }, glm::vec3{ 1, 1, 1 });
    registry.addComponent<Asteroid>(id, target, speed);
    registry.addComponent<CModel>(id, "asteroid");
    registry.addComponent<Health>(id, 3);
    registry.addComponent<CCollider>(id, false, settings::enemy_bitmask);
    registry.addComponent<CSphereBounds>(id, radius);
    registry.addComponent<CDynamicBody>(id,0.5, 0.8f, 0.3f);
    registry.addComponent<CPointLight>(id, glm::vec3{ 0.5, 0.5, 0.5 }, glm::vec3{ 0.9, 0.9, 0.9 }, glm::vec3{ 0.5, 0.5, 0.5 }, 1.0f, 0.09f, 0.032f);
    registry.addComponent<CRepeatAcceleration>(id, glm::vec3{ 0, 0, 1 } * speed, glm::vec3{ -0.2, -0.2, -0.2 });
    registry.addComponent<Damage>(id, 10);
    return id;

}

unsigned short Factory::createSpaceDebris(Registry & registry, glm::vec3 position, glm::vec3 direction, glm::vec3 spin, float speed)
{
    const auto id = registry.createEntity();
    registry.addComponent<CTransform>(id, position, glm::vec3{ 0, 0, 0 }, glm::vec3{ 2, 2, 2 });
    registry.addComponent<CCollider>(id, false, settings::enemy_bitmask);
    registry.addComponent<CDynamicBody>(id,0.5, 0.3f, 0.3f);
    registry.addComponent<CBoxBounds>(id, glm::vec3{ 5, 1, 2.75 });
    registry.addComponent<CModel>(id, "sat");
    registry.addComponent<CRepeatAcceleration>(id, direction * speed, spin);
    registry.addComponent<Damage>(id, 10);
    return id;
}




