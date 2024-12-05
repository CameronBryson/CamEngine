#include "MenuScene.hpp"
#include "User/Scripts/Controllers/MainMenuController.hpp"
#include "User/Scripts/Controllers/SpinningModelController.hpp"
#include "User/Scripts/Factory.hpp"
MenuScene::MenuScene() : BaseScene() { printf("MenuScene created\n"); }
MenuScene::~MenuScene() { printf("MenuScene destroyed\n"); }
void MenuScene::init()
{
	BaseScene::init();
	Factory factory(this);

	factory.createDirectionalLight(glm::vec3{0, -0.2, -1.0}, glm::vec3{0.2, 0.2, 0.2}, glm::vec3{0.5f, 0.5f, 0.5f},
	                               glm::vec3{0.2, 0.2, 0.2});

	auto testModel = createEntity();
	addComponent<CModel>(testModel, "bottle");
	addComponent<CTransform>(testModel, glm::vec3{0, -2, -10}, glm::vec3{0, 0, 0}, glm::vec3{1.0, 1.0, 1.0});
	auto text = mRegistry.createEntity();
	addComponent<CText>(text, "Engine Demo", glm::vec2{200.0f, 100.0f}, 1.0f, glm::vec3{0.3f, 0.5f, 0.7f});

	auto text2 = mRegistry.createEntity();
	addComponent<CText>(text2, "Press Space To Play", glm::vec2{300.0f, 200.0f}, 1.0f, glm::vec3{0.3f, 0.5f, 0.7f});

	auto mainMenuController = mRegistry.createEntity();
	addComponent<MainMenuController>(mainMenuController, this, mainMenuController);
	addComponent<SpinningModelController>(mainMenuController, this, mainMenuController);
}
void MenuScene::lateInit() { BaseScene::lateInit(); }
void MenuScene::update(const float dt) { BaseScene::update(dt); }
void MenuScene::lateUpdate(const float dt) { BaseScene::lateUpdate(dt); }
void MenuScene::render() { BaseScene::render(); }
void MenuScene::lateRender() { BaseScene::lateRender(); }
void MenuScene::shutdown() { BaseScene::shutdown(); }
void MenuScene::lateShutdown() { BaseScene::lateShutdown(); }
void MenuScene::InitSparseSets() {

	mRegistry.createSparseSet<MainMenuController>();
	mRegistry.createSparseSet<SpinningModelController>();
}
