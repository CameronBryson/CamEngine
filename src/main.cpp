#include "pch.hpp"
#include "Engine/Managers/GameManager.hpp"
#include "User/Scenes/PlayScene.hpp"
#include "User/Scenes/TestScene.hpp"
#include "User/Scenes/MenuScene.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main()
{
	GameManager::firstInit();
	GameManager::mCurrentScene = std::make_unique<MenuScene>();
	GameManager::init();
	//GameManager::loadScene<MenuScene>();
	GameManager::gameLoop();
	GameManager::shutdown();
	GameManager::finalShutdown();
	
	return 0;
}
