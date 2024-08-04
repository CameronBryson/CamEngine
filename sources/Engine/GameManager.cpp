#include "GameManager.hpp"
game_manager *game_manager::m_instance_ = nullptr;

game_manager::game_manager()
{
    m_current_scene_ = nullptr;
}

game_manager::~game_manager()
{
    shutdown();
}

game_manager *game_manager::get_instance()
{
    if (m_instance_ == nullptr)
    {
        m_instance_ = new game_manager();
    }
    return m_instance_;
}

void game_manager::init() const
{
    m_current_scene_->init();
}

void game_manager::update(const float dt) const
{
    m_current_scene_->update(dt);
    m_current_scene_->late_update(dt);
}

void game_manager::render() const
{
    m_current_scene_->render();
}

void game_manager::shutdown() const
{
    m_current_scene_->shutdown();
    delete m_current_scene_;
    // delete m_instance;
}