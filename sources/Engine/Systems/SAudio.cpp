#include "SAudio.hpp"

#include "Engine/Base/BaseScene.hpp"

SAudio::SAudio(BaseScene* scene) : mScene(scene)
{
}

void SAudio::init()
{
	mScene->mAudioManager.loadSoundBuffer("TestBuffer", "assets/test.ogg");
	mScene->mAudioManager.loadAudioSource("TestSound", "TestBuffer");
	auto test = mScene->mAudioManager.getAudioSource("TestSound");
	//test->play();
}

void SAudio::update(float deltaTime)
{
	
}

void SAudio::shutdown()
{
}
