#pragma once
#include <Engine/Event.hpp>

#include "Engine/IScene.hpp"

class Camera;
class registry;
class s_camera;
class s_collision;
class s_health;
class s_physics;
class s_render;
class s_ui;
class s_player;
class s_enemy;
class factory;

class play_scene final : public i_scene
{
public:
    play_scene();
    ~play_scene() override;
    void init() override;
    void update(float dt) override;
    void late_update(float dt) override;
    void render() override;
    void shutdown() override;
    registry& get_registry() override;

private:
    void init_sparse_sets();

private:
    std::unique_ptr<registry> m_registry_;
    std::unique_ptr<factory> m_factory_;
    std::unique_ptr<s_player> m_system_player_;
    std::unique_ptr<s_camera> m_system_camera_;
    std::unique_ptr<s_collision> m_system_collision_;
    std::unique_ptr<s_health> m_system_health_;
    std::unique_ptr<s_physics> m_system_physics_;
    std::unique_ptr<s_render> m_system_render_;
    std::unique_ptr<s_ui> m_system_ui_;
    std::unique_ptr<s_enemy> m_system_enemy_;
    std::unique_ptr<Camera> m_camera_;
};
