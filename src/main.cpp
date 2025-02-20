#include "pch.hpp"
#include "Engine/Managers/GameManager.hpp"
#include "TestScene.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <wtypes.h>
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 1;
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
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
