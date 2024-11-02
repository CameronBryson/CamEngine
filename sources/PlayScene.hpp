#pragma once
#include <Engine/Event.hpp>

#include "Engine/IScene.hpp"

class Camera;
class Registry;
class SCamera;
class SCollision;
class SHealth;
class SPhysics;
class SRender;
class SUI;
class SPlayer;
class SEnemy;
class SWaveSpawn;
class SAsteroid;
class SBoundary;
class Factory;
class GraphicsManager;

class PlayScene final : public IScene
{
public:
    PlayScene();
    ~PlayScene() override;
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
    std::unique_ptr<Camera> m_camera_;
    std::unique_ptr<GraphicsManager> m_graphics_manager_;
    std::unique_ptr<SPlayer> m_system_player_;
    std::unique_ptr<SCamera> m_system_camera_;
    std::unique_ptr<SCollision> m_system_collision_;
    std::unique_ptr<SHealth> m_system_health_;
    std::unique_ptr<SPhysics> m_system_physics_;
    std::unique_ptr<SRender> m_system_render_;
    std::unique_ptr<SUI> m_system_ui_;
    std::unique_ptr<SEnemy> m_system_enemy_;
    std::unique_ptr<SWaveSpawn> m_system_wave_spawn_;
    std::unique_ptr<SAsteroid> m_system_asteroid_;
    std::unique_ptr<SBoundary> m_system_boundry_;
};
