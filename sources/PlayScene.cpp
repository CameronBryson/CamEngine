#include "PlayScene.hpp"

#include <Engine/Event.hpp>

#include "Components.hpp"
#include "Engine/CollisionEvents.hpp"
#include "Engine/EventHandler.hpp"
#include "Engine/Factory.hpp"
#include "Engine/Registry.hpp"
#include "Systems/Systems.hpp"
#include "Engine/Timer.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/GraphicsManager.hpp"
#include "Systems/SAsteroid.hpp"
#include "Systems/SBoundry.hpp"
#include "Systems/SEnemy.hpp"
#include "Systems/SWaveSpawn.hpp"
#include "Systems/Systems.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Model.hpp"
#include "Graphics/Material.hpp"
#include "Graphics/Mesh.hpp"
PlayScene::PlayScene()
{
    printf("PlayScene created\n");
    m_camera_ = std::make_unique<Camera>(glm::vec3 {0.0f, 0.0f, 0.0f});
    m_registry_ = std::make_unique<Registry>();
    m_factory_ = std::make_unique<Factory>(getRegistry());
    m_graphics_manager_ = std::make_unique<GraphicsManager>();
    m_system_camera_ = std::make_unique<SCamera>();
    m_system_collision_ = std::make_unique<SCollision>();
    m_system_health_ = std::make_unique<SHealth>();
    m_system_physics_ = std::make_unique<SPhysics>();
    m_system_render_ = std::make_unique<SRender>();
    m_system_ui_ = std::make_unique<SUI>();
    m_system_player_ = std::make_unique<SPlayer>();
    m_system_enemy_ = std::make_unique<SEnemy>();
    m_system_wave_spawn_ = std::make_unique<SWaveSpawn>();
    m_system_asteroid_ = std::make_unique<SAsteroid>();
    m_system_boundry_ = std::make_unique<SBoundary>();
}

PlayScene::~PlayScene()
{
    printf("PlayScene destroyed\n");
}

void PlayScene::init()
{

    initSparseSets();

    m_system_render_->init(*m_graphics_manager_);
    Timer benchmark_timer(Stats::stat_type::BENCHMARK);
    auto player  = m_factory_->createPlayer(*m_registry_);
    settings::player_id = player;

    m_factory_->createBoundary(*m_registry_, glm::vec3{-45,0,0}, glm::vec3{1,45,1});
    m_factory_->createBoundary(*m_registry_, glm::vec3{45,0,0}, glm::vec3{1,45,1});
    m_factory_->createBoundary(*m_registry_, glm::vec3{0,-45,0}, glm::vec3{45,1,1});
    m_factory_->createBoundary(*m_registry_, glm::vec3{0,45,0}, glm::vec3{45,1,1});

    //m_factory_->create_space_debris(*m_registry_,glm::vec3{0,-5,-10}, glm::vec3{0,1,1},glm::vec3{0.4,0.4,0.4},3 );

    //m_factory_->create_quad(*m_registry_, glm::vec3{-10,0,5}, glm::vec3{1,10,10});
    auto skybox = m_factory_->createSkybox(*m_registry_, glm::vec3{0,0,0}, 10);
    //m_factory_->create_asteroid(*m_registry_, glm::vec3{0,3, -35},2, 8.0f, player);
    //m_factory_->create_enemy_ship(*m_registry_,  glm::vec3{0.0f, 3.0f, -50.0f},5, glm::vec3 {0.0f, 0.0f, 1.0f},10.00,player);
    m_factory_->createDirectionalLight(*m_registry_,glm::vec3{0,-0.2,-1.0}, glm::vec3{0.2,0.2,0.2}, glm::vec3{0.1f,0.1f,0.1f}, glm::vec3{0.1,0.1,0.1});
    //m_factory_->create_point_light(*m_registry_,glm::vec3{0,0,0}, glm::vec3{0.7,0.7,0.7}, glm::vec3{1,1,1},glm::vec3{1,1,1},1,0.22,0.2);
    // auto crosshair = m_registry_->create_entity();
    // m_registry_->add_component<c_transform>(crosshair,c_transform{.scale = glm::vec3(0.1f,0.1f,0.1f)});
    // m_registry_->add_component<c_ui>(crosshair,c_ui{.follow_cursor=true});
    m_registry_->processCommands();
    m_camera_->camera_follow_target_ = &m_registry_->getComponent<CTransform>(player);
    m_camera_->skybox_tranform = &m_registry_->getComponent<CTransform>(skybox);
}

void PlayScene::update(const float dt)
{
    Timer update_timer(Stats::stat_type::UPDATE);
    m_registry_->processCollisionEesolutions();
    m_system_camera_->update(*m_camera_, dt);
    m_system_player_->update(*m_registry_, *m_camera_, dt);
    m_system_enemy_->update(*m_registry_, dt);
    m_system_ui_->update(*m_registry_);
    m_system_asteroid_->update(*m_registry_,dt);
    m_system_physics_->update(*m_registry_, dt);
    m_system_health_->update(*m_registry_, dt);
    m_system_boundry_->update(*m_registry_,dt);
    m_system_wave_spawn_->update(*m_registry_,dt);
}

void PlayScene::lateUpdate(const float dt)
{
    m_system_collision_->update(*m_registry_);
    m_registry_->processCommands();
    m_registry_->resetKeyStates();
}

void PlayScene::render()
{
    Timer render_timer(Stats::stat_type::RENDER);
    m_system_render_->update(*m_registry_, *m_graphics_manager_, *m_camera_);
}

void PlayScene::shutdown()
{
    m_system_render_->shutdown();
    m_system_player_->shutdown();
    m_system_physics_->shutdown();
}

Registry & PlayScene::getRegistry()
{
    return *m_registry_;
}

void PlayScene::initSparseSets()
{
    m_registry_->createSparseSet<CTransform>();
    m_registry_->createSparseSet<CRender>();
    m_registry_->createSparseSet<CPlayer>();
    m_registry_->createSparseSet<CCollider>();
    m_registry_->createSparseSet<CHealth>();
    m_registry_->createSparseSet<CEnemy>();
    m_registry_->createSparseSet<CQuad>();
    m_registry_->createSparseSet<CSphere>();
    m_registry_->createSparseSet<CModel>();
    m_registry_->createSparseSet<CDirectionalLight>();
    m_registry_->createSparseSet<CDynamicBody>();
    m_registry_->createSparseSet<CUI>();
    m_registry_->createSparseSet<CDamage>();
    m_registry_->createSparseSet<CBackground>();
    m_registry_->createSparseSet<CAsteroid>();
    m_registry_->createSparseSet<CPointLight>();
    m_registry_->createSparseSet<CRepeatAcceleration>();
}