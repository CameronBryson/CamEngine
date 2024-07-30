#include "PlayScene.hpp"

#include "Components.hpp"
#include "Factory.hpp"
#include "Registry.hpp"
#include "SCamera.hpp"
#include "SCollision.hpp"
#include "Systems.hpp"
#include "Timer.hpp"
#include "raylib.h"
play_scene::play_scene()
{
    printf("PlayScene created\n");
}
play_scene::~play_scene()
{
    printf("PlayScene destroyed\n");
}
void play_scene::init()
{
    m_camera_.position = {0.0f, 50.0f, -50.0f};
    m_camera_.target= {0.0f, 0.0f, 0.0f};
    m_camera_.up = {0.0f, 1.0f, 0.0f};
    m_camera_.fov = 60.0f;
    init_sparse_sets();

    s_render::init();
    timer benchmark_timer(stats::stat_type::BENCHMARK);
    for (int i = 1; i < settings::max_entities - 3; i++)
    {

        const auto entity = m_registry_.create_entity();
        m_registry_.add_component<c_transform>(entity, c_transform());
        m_registry_.add_component<c_rigid_body>(entity, c_rigid_body());
        m_registry_.add_component<c_velocity>(entity, c_velocity());
        m_registry_.add_component<c_player>(entity, c_player());
        m_registry_.add_component<c_aabb>(entity, c_aabb());
    }
    for (int i = 1; i < settings::max_entities - 3; i++)
    {
        m_registry_.delete_entity(i);
    }
    factory::create_player(m_registry_);
    // const auto enemy = m_registry_.create_entity();
    // m_registry_.add_component<c_transform>(enemy, c_transform{.position = {0.0f, 0.0f, 0.0f}});
    // m_registry_.add_component<c_rigid_body>(enemy, c_rigid_body{.drag = 0.9f});
    // m_registry_.add_component<c_velocity>(enemy, c_velocity());
    // m_registry_.add_component<c_enemy>(enemy, c_enemy());
    // m_registry_.add_component<c_aabb>(enemy, c_aabb{.extents = {1.0f, 1.0f, 1.0f}});
    //
    // const auto enemy2 = m_registry_.create_entity();
    // m_registry_.add_component<c_transform>(enemy2, c_transform{.position = {-5.0f, 0.0f, 0.0f}});
    // m_registry_.add_component<c_rigid_body>(enemy2, c_rigid_body{.drag = 0.9f});
    // m_registry_.add_component<c_velocity>(enemy2, c_velocity());
    // m_registry_.add_component<c_enemy>(enemy2, c_enemy());
    // m_registry_.add_component<c_sphere>(enemy2, c_sphere{.radius = 1.0f});
}

void play_scene::update(const float dt)
{
    timer update_timer(stats::stat_type::UPDATE);
    s_camera::update(m_registry_, dt,  m_camera_);
    s_player::update(m_registry_, dt);
    s_physics::update(m_registry_, dt);
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
registry &play_scene::get_registry()
{
    return m_registry_;
}
void play_scene::init_sparse_sets()
{
    m_registry_.create_sparse_set<c_transform>();
    m_registry_.create_sparse_set<c_render>();
    m_registry_.create_sparse_set<c_rigid_body>();
    m_registry_.create_sparse_set<c_player>();
    m_registry_.create_sparse_set<c_sprite>();
    m_registry_.create_sparse_set<c_static_body>();
    m_registry_.create_sparse_set<c_kinetic_body>();
    m_registry_.create_sparse_set<c_velocity>();
    m_registry_.create_sparse_set<c_collider>();
    m_registry_.create_sparse_set<c_health>();
    m_registry_.create_sparse_set<c_enemy>();
    m_registry_.create_sparse_set<c_aabb>();
    m_registry_.create_sparse_set<c_sphere>();
    m_registry_.create_sparse_set<c_capsule>();
}
