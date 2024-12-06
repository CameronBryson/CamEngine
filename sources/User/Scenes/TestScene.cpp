#include "Engine/pch.hpp"
#include "TestScene.hpp"

#include "Engine/Managers/GameManager.hpp"
#include "PlayScene.hpp"
#include "User/Scripts/Controllers/CameraController.hpp"
#include "User/Scripts/Controllers/PlayerController.hpp"
#include "User/Scripts/Controllers/TestSceneController.hpp"
#include "User/Scripts/Factory.hpp"
TestScene::TestScene() : BaseScene() {}
TestScene::~TestScene() {}
void TestScene::init()
{
	Factory factory(this);
	BaseScene::init();

	auto controller = createEntity();
	addComponent<CameraController>(controller, this, controller);

	factory.createDirectionalLight(glm::vec3{0, -0.2, -1.0}, glm::vec3{1.0, 1.0, 1.0}, glm::vec3{0.5f, 0.5f, 0.5f},
	                               glm::vec3{0.2, 0.2, 0.2});

	auto testModel = createEntity();
	addComponent<CModel>(testModel, "bottle");
	addComponent<CTransform>(testModel, glm::vec3{0, 0, -10}, glm::vec3{0, 0, 0}, glm::vec3{2.0, 2.0, 2.0});
}
void TestScene::lateInit() { BaseScene::lateInit(); }
void TestScene::update(float dt) { BaseScene::update(dt); }
void TestScene::lateUpdate(float dt) { BaseScene::lateUpdate(dt); }
void TestScene::render() { BaseScene::render(); }
void TestScene::lateRender()
{
	BaseScene::lateRender();
	GameManager::loadScene<PlayScene>();
}
void TestScene::shutdown() 
{ 
	BaseScene::shutdown();
}
void TestScene::lateShutdown() { BaseScene::lateShutdown(); }

