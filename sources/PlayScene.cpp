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
#include "Systems/Systems.hpp"
play_scene::play_scene()
{
    printf("PlayScene created\n");
    m_camera_ = std::make_unique<Camera>(glm::vec3 {0.0f, 0.0f, -1.50f});
    //m_camera_ = Camera(glm::vec3(0.0f, 0.0f, -1.50f));
    m_registry_ = std::make_unique<registry>();
    m_factory_ = std::make_unique<factory>(get_registry());
    m_system_camera_ = std::make_unique<s_camera>();
    m_system_collision_ = std::make_unique<s_collision>();
    m_system_health_ = std::make_unique<s_health>();
    m_system_physics_ = std::make_unique<s_physics>();
    m_system_render_ = std::make_unique<s_render>();
    m_system_ui_ = std::make_unique<s_ui>();
    m_system_player_ = std::make_unique<s_player>();
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
    // for (int i = 1; i < settings::max_entities - 3; i++)
    // {
    //
    //     const auto entity = m_registry_.create_entity();
    //     m_registry_.add_component<c_transform>(entity, c_transform());
    //     m_registry_.add_component<c_rigid_body>(entity, c_rigid_body());
    //     m_registry_.add_component<c_velocity>(entity, c_velocity());
    //     m_registry_.add_component<c_player>(entity, c_player());
    //     m_registry_.add_component<c_quad>(entity, c_quad());
    // }
    // for (int i = 1; i < settings::max_entities - 3; i++)
    // {
    //     m_registry_.delete_entity(i);
    // }
    m_factory_->create_sphere(*m_registry_, glm::vec3{0.8f, 0.8f, 0.8}, 3.0f);
    m_factory_->create_quad(*m_registry_, glm::vec3{-0.8f, 0.0f, 0.0f}, glm::vec3{3.0f, 0.4f, 0.2f});
    m_factory_->create_enemy_ship(*m_registry_,  glm::vec3{0.0f, 3.0f, -10.0f},5, glm::vec3 {0.0f, 0.0f, 1.0f},0.01);
    auto player  = m_factory_->create_player(*m_registry_);
    m_factory_->create_directional_light(*m_registry_,glm::vec3{0,-0.2,-1.0}, glm::vec3{1.0,1.0,1.0}, glm::vec3{0.5f,0.5f,0.5f}, glm::vec3{1.0,1.0,1.0});
    auto floor = m_registry_->create_entity();
    m_registry_->add_component<c_transform>(floor, c_transform{.position = glm::vec3(0.0f, -3.0f, 0.0f), .rotation = glm::vec3(0.0f, 0.0f, 0.0f)});
    m_registry_->add_component<c_quad>(floor, c_quad{.extents = {10,0.1,10}});
    m_registry_->add_component<c_collider>(floor, c_collider());
    auto crosshair = m_registry_->create_entity();
    m_registry_->add_component<c_transform>(crosshair,c_transform{.scale = glm::vec3(0.1f,0.1f,0.1f)});
    m_registry_->add_component<c_ui>(crosshair,c_ui{.follow_cursor=true});
    m_registry_->process_commands();
    m_camera_->camera_follow_target_ = &m_registry_->get_component<c_transform>(player);
}

void play_scene::update(const float dt)
{
    timer update_timer(stats::stat_type::UPDATE);
    m_registry_->process_collision_resolutions();
    m_system_camera_->update(*m_camera_, dt);
    m_system_player_->update(*m_registry_, dt);
    m_system_physics_->update(*m_registry_, dt);
    m_system_health_->update(*m_registry_, dt);
    m_system_ui_->update(*m_registry_);
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
}