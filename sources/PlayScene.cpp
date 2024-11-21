#include "PlayScene.hpp"
#include "Scripts/PlayerController.hpp"
#include "Scripts/EnemyShipController.hpp"
#include "Scripts/WaveController.hpp"
#include "Scripts/CameraController.hpp"
#include "Scripts/AsteroidController.hpp"
#include "Scripts/BoundaryController.hpp"
#include "Scripts/HealthController.hpp"

#include "Scripts/Asteroid.hpp"
#include "Scripts/EnemyShip.hpp"
#include "Scripts/Player.hpp"
#include "Scripts/Health.hpp"
#include "Scripts/Damage.hpp"

#include <Engine/Event.hpp>

PlayScene::PlayScene() : mFactory(mRegistry),mCollisionSystem(&mRegistry)
{
    printf("PlayScene created\n");

}

PlayScene::~PlayScene()
{
    printf("PlayScene destroyed\n");
}

void PlayScene::init()
{

    initSparseSets();

    mRenderSystem.init(mGraphicsManager);
	mCollisionSystem.init();
    Timer benchmark_timer(Stats::stat_type::BENCHMARK);
    auto player  = mFactory.createPlayer(mRegistry);
    mRegistry.addComponent<PlayerController>(player, &mCamera, &mRegistry, player);
    settings::player_id = player;

    auto controller = mRegistry.createEntity();
    mRegistry.addComponent<WaveController>(controller, &mRegistry, controller);
    mRegistry.addComponent<EnemyShipController>(controller, &mRegistry, controller);
    mRegistry.addComponent<AsteroidController>(controller, &mRegistry, controller);
    mRegistry.addComponent<CameraController>(controller, &mCamera, &mRegistry, controller);
    mRegistry.addComponent<BoundaryController>(controller, &mRegistry, controller);
    mRegistry.addComponent<HealthController>(controller, &mRegistry, controller);


    mFactory.createBoundary(mRegistry, glm::vec3{-45,0,0}, glm::vec3{1,45,1});
    mFactory.createBoundary(mRegistry, glm::vec3{45,0,0}, glm::vec3{1,45,1});
    mFactory.createBoundary(mRegistry, glm::vec3{0,-45,0}, glm::vec3{45,1,1});
    mFactory.createBoundary(mRegistry, glm::vec3{0,45,0}, glm::vec3{45,1,1});

    auto skybox = mFactory.createSkybox(mRegistry, glm::vec3{0,0,0}, 10);
    mFactory.createDirectionalLight(mRegistry,glm::vec3{0,-0.2,-1.0}, glm::vec3{0.6,0.6,0.6}, glm::vec3{0.5f,0.5f,0.5f}, glm::vec3{0.2,0.2,0.2});

    mRegistry.processCommands();
    mCamera.camera_follow_target_ = &mRegistry.getComponent<CTransform>(player);
    mCamera.skybox_tranform = &mRegistry.getComponent<CTransform>(skybox);

    EventHandler::GetInstance()->scriptDispatcher.SendEvent(InitEvent());
    EventHandler::GetInstance()->scriptDispatcher.SendEvent(LateInitEvent());
}

void PlayScene::update(const float dt)
{
    Timer update_timer(Stats::stat_type::UPDATE);
    mRegistry.processCollisionResolutions();
    mPhysicsSystem.update(mRegistry, dt);
    EventHandler::GetInstance()->scriptDispatcher.SendEvent(UpdateEvent(dt));

}

void PlayScene::lateUpdate(const float dt)
{
    mCollisionSystem.update(mRegistry);
    mRegistry.processCommands();
    mRegistry.resetKeyStates();
    EventHandler::GetInstance()->scriptDispatcher.SendEvent(LateUpdateEvent(dt));
}

void PlayScene::render()
{
    Timer render_timer(Stats::stat_type::RENDER);
    mRenderSystem.update(mRegistry, mGraphicsManager, mCamera);
}

void PlayScene::shutdown()
{
    mRenderSystem.shutdown();
    mPhysicsSystem.shutdown();
    EventHandler::GetInstance()->scriptDispatcher.SendEvent(ShutdownEvent());
}

Registry & PlayScene::getRegistry()
{
    return mRegistry;
}

void PlayScene::initSparseSets()
{
    mRegistry.createSparseSet<CTransform>();
    mRegistry.createSparseSet<CRender>();
    mRegistry.createSparseSet<Player>();
    mRegistry.createSparseSet<CCollider>();
    mRegistry.createSparseSet<Health>();
    mRegistry.createSparseSet<EnemyShip>();
    mRegistry.createSparseSet<CBoxBounds>();
    mRegistry.createSparseSet<CSphereBounds>();
    mRegistry.createSparseSet<CModel>();
    mRegistry.createSparseSet<CDirectionalLight>();
    mRegistry.createSparseSet<CDynamicBody>();
    mRegistry.createSparseSet<CUI>();
    mRegistry.createSparseSet<Damage>();
    mRegistry.createSparseSet<CBackground>();
    mRegistry.createSparseSet<Asteroid>();
    mRegistry.createSparseSet<CPointLight>();
    mRegistry.createSparseSet<CRepeatAcceleration>();
    mRegistry.createSparseSet<PlayerController>();
    mRegistry.createSparseSet<WaveController>();
    mRegistry.createSparseSet<EnemyShipController>();
    mRegistry.createSparseSet<AsteroidController>();
    mRegistry.createSparseSet<CameraController>();
    mRegistry.createSparseSet<BoundaryController>();
    mRegistry.createSparseSet<HealthController>();
}