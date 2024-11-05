#include "Factory.hpp"
#include "Engine/Event.hpp"
#include "Engine/FactoryEvents.hpp"
#include "Registry.hpp"
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
    registry.addComponent<CPlayer>(id, CPlayer());
    registry.addComponent<CHealth>(id, CHealth{.health = 100});
    registry.addComponent<CTransform>(id, CTransform{
            .position = {0, 0, 0}, .rotation = {0, 3.14, 0}, .scale = {0.1, 0.1, 0.1}
    });
    registry.addComponent<CSphere>(id, CSphere{.radius = 5.0f});
    //registry.add_component<c_quad>(id, c_quad{.extents = {3.0f, 3.0f, 3.0f}});
    registry.addComponent<CCollider>(id, CCollider{.is_trigger = true,.collision_bitmask = settings::player_bitmask});
    registry.addComponent<CModel>(id, CModel{.name = "player"});
    registry.addComponent<CDynamicBody>(id,CDynamicBody{.drag = 0.4f,.elasticity = 0.1f, .angluar_drag = 0.9f});
    registry.addComponent<CDamage>(id, CDamage{.damage = 1});
    registry.addComponent<CPointLight>(id, CPointLight{.ambient = {0.5,0.5,0.5},.diffuse = {0.9,0.9,0.9},.specular = {0.5,0.5,0.5},.constant = 1,.linear = 0.09,.quadratic = 0.032});

    return id;

}
unsigned short Factory::createSkybox(Registry &registry, glm::vec3 position, float radius) {
    const auto id = registry.createEntity();
    registry.addComponent<CTransform>(id, CTransform{.position = position, .scale = {5, 5, 5}});
    registry.addComponent<CModel>(id, CModel{.name = "skybox"});
    registry.addComponent<CBackground>(id,CBackground());
    return id;
}
unsigned short Factory::createBoundary(Registry &registry, glm::vec3 position, glm::vec3 extents) {
    const auto id = registry.createEntity();
    registry.addComponent<CTransform>(id, CTransform{.position = position,.rotation = {0, 0, 0}, .scale = {1.0f,1.0f,1.0f}});
    registry.addComponent<CQuad>(id, CQuad{.extents = extents});
    registry.addComponent<CCollider>(id, CCollider{ .collision_bitmask = settings::enemy_bitmask});
    return id;
}
unsigned short Factory::createDirectionalLight(Registry &registry, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse,
                                       glm::vec3 specular) {
    const auto id =registry.createEntity();
    registry.addComponent<CDirectionalLight>(id, CDirectionalLight{.direction = direction, .ambient = ambient, .diffuse = diffuse, .specular = specular} );
    return id;
}
unsigned short Factory::createPointLight(Registry & registry, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic)
{
    const auto id = registry.createEntity();
    registry.addComponent<CTransform>(id, CTransform{.position = position});
    registry.addComponent<CPointLight>(id, CPointLight{.ambient = ambient,.diffuse = diffuse,.specular = specular,.constant = constant,.linear = linear,.quadratic = quadratic});

    return id;
}

unsigned short Factory::createProjectile(Registry &registry, glm::vec3 position, glm::vec3 direction, float speed,unsigned int collision_bitmask) {
    const auto id = registry.createEntity();
    registry.addComponent<CTransform>(id, CTransform{.position = position, .scale = {0.5,0.5,0.5}});
    registry.addComponent<CDamage>(id, CDamage{.damage = 1});
    registry.addComponent<CSphere>(id, CSphere{.radius = 1});
    registry.addComponent<CHealth>(id, CHealth{.health =  1});
    registry.addComponent<CModel>(id, CModel{.name = "sphere"});
    registry.addComponent<CCollider>(id, CCollider{.collision_bitmask = collision_bitmask});
    registry.addComponent<CDynamicBody>(id, CDynamicBody{.drag = 0.0f, .velocity = direction * speed,.angluar_drag = 0.0f});
    registry.addComponent<CPointLight>(id, CPointLight{.ambient = {0.4,0.4,0.4},.diffuse = {0.9,0.9,0.9},.specular = {0.8,0.8,0.8},.constant = 1,.linear = 0.14,.quadratic = 0.07});

    //registry.add_component<c_projectile>(id, c_projectile{.direction = direction, .speed = speed});
    return id;
}
unsigned short Factory::createEnemyShip(Registry &registry, glm::vec3 position, float radius, glm::vec3 direction,
                                float speed, unsigned short target) {
    const auto id = registry.createEntity();
    registry.addComponent<CTransform>(id, CTransform{.position = position,.rotation = {0,-3.14/2,0}, .scale = {0.5f,0.5f,0.5f}});
    registry.addComponent<CHealth>(id, CHealth{.health = 1});
    registry.addComponent<CEnemy>(id, CEnemy{.target =  target,.speed = speed,.direction = direction});
    registry.addComponent<CQuad>(id, CQuad{.extents ={8,2,8}});
    registry.addComponent<CModel>(id, CModel{.name = "enemy"});
    registry.addComponent<CCollider>(id, CCollider{ .collision_bitmask = settings::enemy_bitmask});
    registry.addComponent<CDynamicBody>(id, CDynamicBody{.drag = 0.8f});
    registry.addComponent<CDamage>(id, CDamage{.damage = 1});
    registry.addComponent<CPointLight>(id, CPointLight{.ambient = {0.5,0.5,0.5},.diffuse = {1,1,1},.specular = {1,1,1},.constant = 1,.linear = 0.14,.quadratic = 0.07});

    return id;
}
unsigned short Factory::createAsteroid(Registry & registry, glm::vec3 position, float radius, float speed, unsigned short target)
{
    const auto id = registry.createEntity();
    registry.addComponent<CTransform>(id, CTransform{.position =  position,.scale = {1,1,1}});
    registry.addComponent<CAsteroid>(id, CAsteroid{.target = target,.speed = speed});
    registry.addComponent<CModel>(id, CModel{.name = "asteroid"});
    registry.addComponent<CHealth>(id, CHealth{.health = 3});
    registry.addComponent<CCollider>(id, CCollider{.collision_bitmask = settings::enemy_bitmask});
    registry.addComponent<CSphere>(id, CSphere{.radius = radius});
    registry.addComponent<CDynamicBody>(id, CDynamicBody{.drag = 0.8f,.angluar_drag = 0.3});
    registry.addComponent<CPointLight>(id, CPointLight{.ambient = {0.5,0.5,0.5},.diffuse = {0.9,0.9,0.9},.specular = {0.5,0.5,0.5},.constant = 1,.linear = 0.09,.quadratic = 0.032});
    registry.addComponent<CRepeatAcceleration>(id, CRepeatAcceleration{.acceleration = glm::vec3{0,0,1} * speed,.angularAcceleration = {-0.2,-0.2,-0.2}});
    registry.addComponent<CDamage>(id,CDamage{.damage = 10});
    return id;

}

unsigned short Factory::createSpaceDebris(Registry & registry, glm::vec3 position, glm::vec3 direction, glm::vec3 spin, float speed)
{
    const auto id = registry.createEntity();
    registry.addComponent<CTransform>(id, CTransform{.position = position,.scale = {2,2,2}});
    registry.addComponent<CCollider>(id, CCollider{.collision_bitmask = settings::enemy_bitmask});
    registry.addComponent<CDynamicBody>(id,CDynamicBody{.drag = 0.3,.angluar_drag = 0.3});
    registry.addComponent<CQuad>(id, CQuad{.extents = {5,1,2.75}});
    registry.addComponent<CModel>(id, CModel{.name = "sat"});
    //registry.add_component<c_point_light>(id, c_point_light{.ambient = {0.5,0.5,0.5},.diffuse = {0.9,0.9,0.9},.specular = {0.5,0.5,0.5},.constant = 1,.linear = 0.09,.quadratic = 0.032});
    registry.addComponent<CRepeatAcceleration>(id, CRepeatAcceleration{.acceleration = direction * speed,.angularAcceleration = spin});
    registry.addComponent<CDamage>(id,CDamage{.damage = 10});
    return id;
}




