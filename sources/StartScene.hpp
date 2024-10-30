#pragma once
#include <Engine/IScene.hpp>
class Camera;
class s_render;
class graphics_manager;
class start_scene final : public i_scene {
public:
    start_scene();
    ~start_scene() override;
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
    std::unique_ptr<s_render> m_system_render_;
    std::unique_ptr<Camera> m_camera_;
    std::unique_ptr<graphics_manager> m_graphics_manager_;

};
