#include "Engine/Managers/GameManager.hpp"
#include "User/Scenes/PlayScene.hpp"
#include "User/Scenes/TestScene.hpp"
#include "Engine/Util/OpenALUtil.hpp"
#include "Engine/Util/OpenGLUtil.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main()
{
	OpenGlUtil::init();
	OpenALUtil::init();
	GameManager::loadScene<TestScene>();
	GameManager::gameLoop();
	GameManager::shutdown();
	OpenALUtil::shutdown();
	OpenGlUtil::shutdown();
	return 0;
}