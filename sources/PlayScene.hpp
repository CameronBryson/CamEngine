#pragma once
#include "IScene.hpp"
#include "Registry.hpp"
#include "raylib.h"
class play_scene final : public i_scene
{
  public:
    play_scene();
    ~play_scene() override;
    void init() override;
    void update(float dt) override;
    void render() override;
    void shutdown() override;
    registry &get_registry() override;

  private:
    void init_sparse_sets();

  private:
    registry m_registry_;
    Camera3D m_camera_{};
};
