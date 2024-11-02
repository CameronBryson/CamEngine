#pragma once
#include <Engine/IScene.hpp>
class Camera;
class SRender;
class GraphicsManager;
class StartScene final : public IScene {
public:
    StartScene();
    ~StartScene() override;
    void init() override;
    void update(float dt) override;
    void lateUpdate(float dt) override;
    void render() override;
    void shutdown() override;
    Registry& getRegistry() override;
private:
    void initSparseSets();
private:
    std::unique_ptr<Registry> m_registry_;
    std::unique_ptr<Factory> m_factory_;
    std::unique_ptr<SRender> m_system_render_;
    std::unique_ptr<Camera> m_camera_;
    std::unique_ptr<GraphicsManager> m_graphics_manager_;

};
