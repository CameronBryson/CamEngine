#include "pch.hpp"
#include "Engine/Managers/GameManager.hpp"
#include "TestScene.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main()
{
	GameManager::firstInit();
	GameManager::mCurrentScene = std::make_unique<TestScene>();
	GameManager::init();
	//GameManager::loadScene<MenuScene>();
	GameManager::gameLoop();
	GameManager::shutdown();
	GameManager::finalShutdown();
	
	return 0;
}
