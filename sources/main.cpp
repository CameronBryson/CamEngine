#include "Engine/GameManager.hpp"
#include "PlayScene.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main()
{
	GameManager::loadScene<PlayScene>();
	GameManager::init();
	GameManager::gameLoop();
	GameManager::shutdown();
	return 0;
}