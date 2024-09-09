#pragma once
#include <Engine/Event.hpp>

#include "Engine/IScene.hpp"
#include "Engine/Registry.hpp"
#include "Graphics/Camera.hpp"
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
    registry m_registry_;
    Camera m_camera_;
};
