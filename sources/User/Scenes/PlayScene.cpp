#include "Engine/Managers/GameManager.hpp"
#include "Engine/Util/GameSettings.hpp"
#include "PlayScene.hpp"
#include "TestScene.hpp"
#include "User/Scripts/Controllers/AsteroidController.hpp"
#include "User/Scripts/Controllers/BoundaryController.hpp"
#include "User/Scripts/Controllers/CameraController.hpp"
#include "User/Scripts/Controllers/EnemyShipController.hpp"
#include "User/Scripts/Controllers/HealthController.hpp"
#include "User/Scripts/Controllers/PlayerController.hpp"
#include "User/Scripts/Controllers/WaveController.hpp"
#include <Engine/Components.hpp>


PlayScene::PlayScene() : BaseScene(), mFactory(this) { printf("PlayScene created\n"); }

PlayScene::~PlayScene() { printf("PlayScene destroyed\n"); }

void PlayScene::init()
{
	BaseScene::init();

	auto player = mFactory.createPlayer();
	addComponent<PlayerController>(player, this, player);
	settings::player_id = player;

	auto controller = createEntity();
	addComponent<WaveController>(controller, this, controller);
	addComponent<EnemyShipController>(controller, this, controller);
	addComponent<AsteroidController>(controller, this, controller);
	addComponent<CameraController>(controller, this, controller);
	addComponent<BoundaryController>(controller, this, controller);
	addComponent<HealthController>(controller, this, controller);

	//auto text = mRegistry.createEntity();
	//addComponent<CText>(text, "Test Hello World 123 ABC", glm::vec2{200.0f, 100.0f}, 1.0f, glm::vec3{0.3f, 0.5f, 0.7f});


	mFactory.createBoundary(glm::vec3{-45, 0, 0}, glm::vec3{1, 45, 1});
	mFactory.createBoundary(glm::vec3{45, 0, 0}, glm::vec3{1, 45, 1});
	mFactory.createBoundary(glm::vec3{0, -45, 0}, glm::vec3{45, 1, 1});
	mFactory.createBoundary(glm::vec3{0, 45, 0}, glm::vec3{45, 1, 1});

	auto skybox = mFactory.createSkybox(glm::vec3{0, 0, 0}, 10);
	mFactory.createDirectionalLight(glm::vec3{0, -0.2, -1.0}, glm::vec3{0.0, 0.0, 0.0}, glm::vec3{0.1f, 0.1f, 0.1f},
	                                glm::vec3{0.2, 0.2, 0.2});

	mCommandManager.processCommands();
	mMainCamera.camera_follow_target_ = &getComponent<CTransform>(player);
	mMainCamera.skybox_tranform = &getComponent<CTransform>(skybox);
}

void PlayScene::lateInit() { BaseScene::lateInit(); }

void PlayScene::update(const float dt) { BaseScene::update(dt); }

void PlayScene::lateUpdate(const float dt) { BaseScene::lateUpdate(dt); }

void PlayScene::render() { BaseScene::render(); }

void PlayScene::lateRender()
{
	BaseScene::lateRender();
	//GameManager::loadScene<TestScene>();
}

void PlayScene::lateShutdown() { BaseScene::lateShutdown(); }

void PlayScene::shutdown() { BaseScene::shutdown(); }
