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
#include "Systems/SAsteroid.hpp"
#include "Systems/SBoundry.hpp"
#include "Systems/SEnemy.hpp"
#include "Systems/SWaveSpawn.hpp"
#include "Systems/Systems.hpp"
play_scene::play_scene()
{
    printf("PlayScene created\n");
    m_camera_ = std::make_unique<Camera>(glm::vec3 {0.0f, 0.0f, 0.0f});
    m_registry_ = std::make_unique<registry>();
    m_factory_ = std::make_unique<factory>(get_registry());
    m_system_camera_ = std::make_unique<s_camera>();
    m_system_collision_ = std::make_unique<s_collision>();
    m_system_health_ = std::make_unique<s_health>();
    m_system_physics_ = std::make_unique<s_physics>();
    m_system_render_ = std::make_unique<s_render>();
    m_system_ui_ = std::make_unique<s_ui>();
    m_system_player_ = std::make_unique<s_player>();
    m_system_enemy_ = std::make_unique<s_enemy>();
    m_system_wave_spawn_ = std::make_unique<s_wave_spawn>();
    m_system_asteroid_ = std::make_unique<s_asteroid>();
    m_system_boundry_ = std::make_unique<s_boundry>();
}

play_scene::~play_scene()
{
    printf("PlayScene destroyed\n");
}

void play_scene::init()
{

    init_sparse_sets();

    m_system_render_->init();
    timer benchmark_timer(stats::stat_type::BENCHMARK);
    auto player  = m_factory_->create_player(*m_registry_);
    //m_factory_->create_boundry(*m_registry_, glm::vec3{-20,0,0}, glm::vec3{1,10,1});
    //m_factory_->create_boundry(*m_registry_, glm::vec3{20,0,0}, glm::vec3{1,10,1});
    //m_factory_->create_boundry(*m_registry_, glm::vec3{0,-10,0}, glm::vec3{20,1,1});
    //m_factory_->create_boundry(*m_registry_, glm::vec3{0,10,0}, glm::vec3{20,1,1});

    //m_factory_->create_quad(*m_registry_, glm::vec3{-10,0,5}, glm::vec3{1,10,10});
    auto skybox = m_factory_->create_skybox(*m_registry_, glm::vec3{0,0,0}, 10);
    m_factory_->create_asteroid(*m_registry_, glm::vec3{0,3, -35},2, 8.0f, player);
    m_factory_->create_enemy_ship(*m_registry_,  glm::vec3{0.0f, 3.0f, -50.0f},5, glm::vec3 {0.0f, 0.0f, 1.0f},10.00,player);
    m_factory_->create_directional_light(*m_registry_,glm::vec3{0,-0.2,-1.0}, glm::vec3{0.2,0.2,0.2}, glm::vec3{0.1f,0.1f,0.1f}, glm::vec3{0.1,0.1,0.1});
    //m_factory_->create_point_light(*m_registry_,glm::vec3{0,0,0}, glm::vec3{0.7,0.7,0.7}, glm::vec3{1,1,1},glm::vec3{1,1,1},1,0.22,0.2);
    // auto crosshair = m_registry_->create_entity();
    // m_registry_->add_component<c_transform>(crosshair,c_transform{.scale = glm::vec3(0.1f,0.1f,0.1f)});
    // m_registry_->add_component<c_ui>(crosshair,c_ui{.follow_cursor=true});
    m_registry_->process_commands();
    m_camera_->camera_follow_target_ = &m_registry_->get_component<c_transform>(player);
    m_camera_->skybox_tranform = &m_registry_->get_component<c_transform>(skybox);
}

void play_scene::update(const float dt)
{
    timer update_timer(stats::stat_type::UPDATE);
    m_registry_->process_collision_resolutions();
    m_system_camera_->update(*m_camera_, dt);
    m_system_player_->update(*m_registry_, *m_camera_, dt);
    m_system_enemy_->update(*m_registry_, dt);
    m_system_ui_->update(*m_registry_);
    m_system_asteroid_->update(*m_registry_,dt);
    m_system_physics_->update(*m_registry_, dt);
    m_system_health_->update(*m_registry_, dt);
    m_system_boundry_->update(*m_registry_,dt);
}

void play_scene::late_update(const float dt)
{
    m_system_collision_->update(*m_registry_);
    m_registry_->process_commands();
    m_registry_->reset_key_states();
}

void play_scene::render()
{
    timer render_timer(stats::stat_type::RENDER);
    m_system_render_->update(*m_registry_, *m_camera_);
}

void play_scene::shutdown()
{
    m_system_render_->shutdown();
    m_system_player_->shutdown();
    m_system_physics_->shutdown();
}

registry & play_scene::get_registry()
{
    return *m_registry_;
}

void play_scene::init_sparse_sets()
{
    m_registry_->create_sparse_set<c_transform>();
    m_registry_->create_sparse_set<c_render>();
    m_registry_->create_sparse_set<c_player>();
    m_registry_->create_sparse_set<c_collider>();
    m_registry_->create_sparse_set<c_health>();
    m_registry_->create_sparse_set<c_enemy>();
    m_registry_->create_sparse_set<c_quad>();
    m_registry_->create_sparse_set<c_sphere>();
    m_registry_->create_sparse_set<c_capsule>();
    m_registry_->create_sparse_set<c_model>();
    m_registry_->create_sparse_set<c_directional_light>();
    m_registry_->create_sparse_set<c_dynamic_body>();
    m_registry_->create_sparse_set<c_ui>();
    m_registry_->create_sparse_set<c_damage>();
    m_registry_->create_sparse_set<c_background>();
    m_registry_->create_sparse_set<c_asteroid>();
    m_registry_->create_sparse_set<c_point_light>();
}