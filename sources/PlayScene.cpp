#include "PlayScene.hpp"

#include <Engine/Event.hpp>

#include "Components.hpp"
#include "Engine/CollisionEvents.hpp"
#include "Engine/EventHandler.hpp"
#include "Engine/Factory.hpp"
#include "Engine/Registry.hpp"
#include "Systems/Systems.hpp"
#include "Engine/Timer.hpp"
play_scene::play_scene()
{
    printf("PlayScene created\n");
    m_camera_ = Camera(glm::vec3(0.0f, 0.0f, -1.50f));
}

play_scene::~play_scene()
{
    printf("PlayScene destroyed\n");
}

void play_scene::init()
{

    init_sparse_sets();

    s_render::init();
    factory::bind_events();
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
    factory::create_sphere(m_registry_, glm::vec3{0.8f, 0.8f, 0.8}, 3.0f);
    factory::create_quad(m_registry_, glm::vec3{-0.8f, 0.0f, 0.0f}, glm::vec3{3.0f, 0.4f, 0.2f});
    auto player = factory::create_player(m_registry_);
    factory::create_directional_light(m_registry_,glm::vec3{0,-0.2,-1.0}, glm::vec3{1.0,1.0,1.0}, glm::vec3{0.5f,0.5f,0.5f}, glm::vec3{1.0,1.0,1.0});
    auto floor = m_registry_.create_entity();
    m_registry_.add_component<c_transform>(floor, c_transform{.position = glm::vec3(0.0f, -3.0f, 0.0f), .rotation = glm::vec3(0.0f, 0.0f, 0.0f)});
    m_registry_.add_component<c_quad>(floor, c_quad{.extents = {10,0.1,10}});
    m_registry_.add_component<c_collider>(floor, c_collider());
    auto crosshair = m_registry_.create_entity();
    m_registry_.add_component<c_transform>(crosshair,c_transform{.scale = glm::vec3(0.1f,0.1f,0.1f)});
    m_registry_.add_component<c_ui>(crosshair,c_ui{.follow_cursor=true});
    m_registry_.process_commands();
    m_camera_.camera_follow_target_ = &m_registry_.get_component<c_transform>(player);
}

void play_scene::update(const float dt)
{
    timer update_timer(stats::stat_type::UPDATE);
    m_registry_.process_collision_resolutions();
    s_camera::update(m_camera_, dt);
    s_player::update(m_registry_, dt);
    s_physics::update(m_registry_, dt);
    s_ui::update(m_registry_);
}

void play_scene::late_update(const float dt)
{
    s_collision::update(m_registry_);
    m_registry_.process_commands();
}

void play_scene::render()
{
    timer render_timer(stats::stat_type::RENDER);
    s_render::update(m_registry_, m_camera_);
}

void play_scene::shutdown()
{
    s_render::shutdown();
    s_player::shutdown();
    s_physics::shutdown();
}

registry & play_scene::get_registry()
{
    return m_registry_;
}

void play_scene::init_sparse_sets()
{
    m_registry_.create_sparse_set<c_transform>();
    m_registry_.create_sparse_set<c_render>();
    m_registry_.create_sparse_set<c_player>();
    m_registry_.create_sparse_set<c_collider>();
    m_registry_.create_sparse_set<c_health>();
    m_registry_.create_sparse_set<c_enemy>();
    m_registry_.create_sparse_set<c_quad>();
    m_registry_.create_sparse_set<c_sphere>();
    m_registry_.create_sparse_set<c_capsule>();
    m_registry_.create_sparse_set<c_model>();
    m_registry_.create_sparse_set<c_directional_light>();
    m_registry_.create_sparse_set<c_dynamic_body>();
    m_registry_.create_sparse_set<c_ui>();
}