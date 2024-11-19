#pragma once
#include <Engine/Event.hpp>

#include "Engine/IScene.hpp"

#include "Components.hpp"
#include "Engine/CollisionEvents.hpp"
#include "Engine/EventHandler.hpp"
#include "Engine/Factory.hpp"
#include "Engine/Registry.hpp"
#include "Systems/Systems.hpp"
#include "Engine/Timer.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/GraphicsManager.hpp"
#include "Systems/SAsteroid.hpp"
#include "Systems/SBoundry.hpp"
#include "Systems/SEnemy.hpp"
#include "Systems/SWaveSpawn.hpp"
#include "Systems/Systems.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/Model.hpp"
#include "Graphics/Material.hpp"
#include "Graphics/Mesh.hpp"

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
    Registry mRegistry;
    Factory mFactory;
    Camera mCamera;
    GraphicsManager mGraphicsManager;
    SPlayer mSystemPlayer;
    SCamera m_system_camera_;
    SCollision m_system_collision_;
    SHealth m_system_health_;
    SPhysics m_system_physics_;
    SRender m_system_render_;
    SUI m_system_ui_;
    SEnemy m_system_enemy_;
    SWaveSpawn m_system_wave_spawn_;
    SAsteroid m_system_asteroid_;
    SBoundary m_system_boundry_;
};
