#include "MenuScene.hpp"
#include "User/Scripts/Controllers/MainMenuController.hpp"
MenuScene::MenuScene() : BaseScene() { printf("MenuScene created\n"); }
MenuScene::~MenuScene() { printf("MenuScene destroyed\n"); }
void MenuScene::init()
{
	BaseScene::init();
	auto text = mRegistry.createEntity();
	addComponent<CText>(text, "Menu Scene", glm::vec2{200.0f, 100.0f}, 1.0f, glm::vec3{0.3f, 0.5f, 0.7f});

	auto mainMenuController = mRegistry.createEntity();
	addComponent<MainMenuController>(mainMenuController, this, mainMenuController);
}
void MenuScene::lateInit() { BaseScene::lateInit(); }
void MenuScene::update(const float dt) { BaseScene::update(dt); }
void MenuScene::lateUpdate(const float dt) { BaseScene::lateUpdate(dt); }
void MenuScene::render() { BaseScene::render(); }
void MenuScene::lateRender() { BaseScene::lateRender(); }
void MenuScene::shutdown() { BaseScene::shutdown(); }
void MenuScene::lateShutdown() { BaseScene::lateShutdown(); }
void MenuScene::InitSparseSets()
{
	
}
