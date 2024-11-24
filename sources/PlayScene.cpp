#include "PlayScene.hpp"

#include "GameSettings.hpp"
#include "Scripts/PlayerController.hpp"
#include "Scripts/EnemyShipController.hpp"
#include "Scripts/WaveController.hpp"
#include "Scripts/CameraController.hpp"
#include "Scripts/AsteroidController.hpp"
#include "Scripts/BoundaryController.hpp"
#include "Scripts/HealthController.hpp"



PlayScene::PlayScene() : BaseScene(), mFactory(this)
{
    printf("PlayScene created\n");

}

PlayScene::~PlayScene()
{
    printf("PlayScene destroyed\n");
}

void PlayScene::init()
{
	BaseScene::init();

    auto player  = mFactory.createPlayer();
    addComponent<PlayerController>(player,this, player);
    settings::player_id = player;

    auto controller = createEntity();
    addComponent<WaveController>(controller, this, controller);
    addComponent<EnemyShipController>(controller, this, controller);
    addComponent<AsteroidController>(controller, this, controller);
    addComponent<CameraController>(controller, this, controller);
    addComponent<BoundaryController>(controller, this, controller);
    addComponent<HealthController>(controller, this, controller);


    mFactory.createBoundary(glm::vec3{-45,0,0}, glm::vec3{1,45,1});
    mFactory.createBoundary(glm::vec3{45,0,0}, glm::vec3{1,45,1});
    mFactory.createBoundary(glm::vec3{0,-45,0}, glm::vec3{45,1,1});
    mFactory.createBoundary(glm::vec3{0,45,0}, glm::vec3{45,1,1});

    auto skybox = mFactory.createSkybox(glm::vec3{0,0,0}, 10);
    mFactory.createDirectionalLight(glm::vec3{0,-0.2,-1.0}, glm::vec3{0.6,0.6,0.6}, glm::vec3{0.5f,0.5f,0.5f}, glm::vec3{0.2,0.2,0.2});

    mCommandManager.processCommands();
    mMainCamera.camera_follow_target_ = &getComponent<CTransform>(player);
    mMainCamera.skybox_tranform = &getComponent<CTransform>(skybox);
    

}

void PlayScene::lateInit()
{
	BaseScene::lateInit();
    
}

void PlayScene::update(const float dt)
{
	BaseScene::update(dt);
}

void PlayScene::lateUpdate(const float dt)
{
	BaseScene::lateUpdate(dt);
}

void PlayScene::render()
{
	BaseScene::render();
}

void PlayScene::shutdown()
{
	BaseScene::shutdown();
}
