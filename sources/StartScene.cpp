//
// Created by cam on 29/10/24.
//

#include "StartScene.hpp"

#include <Systems/SRender.hpp>
#include <Graphics/Camera.hpp>

start_scene::start_scene()
{
    printf("Start Scene created\n");
    m_camera_ = std::make_unique<Camera>(glm::vec3 {0.0f, 0.0f, 0.0f});
    m_registry_ = std::make_unique<registry>();
    m_factory_ = std::make_unique<factory>(get_registry());
    m_system_render_ = std::make_unique<s_render>();
}

start_scene::~start_scene()
{
    printf("Start Scene destroyed\n");
}

void start_scene::init()
{
    init_sparse_sets();
    m_system_render_->init();
    m_factory_->create_boundry(*m_registry_, glm::vec3{-20,0,0}, glm::vec3{1,10,1});
}

void start_scene::update(float dt)
{
}

void start_scene::late_update(float dt)
{
}

void start_scene::render()
{
    m_system_render_->update(get_registry(), *m_camera_);
}

void start_scene::shutdown()
{
}

registry & start_scene::get_registry()
{
    return *m_registry_;
}

void start_scene::init_sparse_sets()
{
    m_registry_->create_sparse_set<c_transform>();
    m_registry_->create_sparse_set<c_model>();
    m_registry_->create_sparse_set<c_ui>();
    m_registry_->create_sparse_set<c_directional_light>();
    m_registry_->create_sparse_set<c_point_light>();
    m_registry_->create_sparse_set<c_background>();
    m_registry_->create_sparse_set<c_quad>();
    m_registry_->create_sparse_set<c_sphere>();
    m_registry_->create_sparse_set<c_collider>();

}