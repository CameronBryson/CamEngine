#include "Engine/pch.hpp"
#include "SAudio.hpp"
#include "Engine/Managers/GameManager.hpp"
#include "Engine/Managers/AudioManager.hpp"
#include "Engine/Base/BaseScene.hpp"
#include "Engine/Audio/AudioSource.hpp"

SAudio::SAudio(BaseScene* scene) : mScene(scene)
{
}

void SAudio::init()
{
	
	/*auto test = GameManager::mAudioManager->getAudioSource("TestSound");	
	test->play();*/
}

void SAudio::update(float deltaTime)
{
	
}

void SAudio::shutdown()
{
}
