#include "AudioManager.hpp"

#include <iostream>
#include <ostream>

#include "Engine/Util/OpenALUtil.hpp"

AudioManager::~AudioManager() { clear(); }

void AudioManager::loadResources()
{
	loadSoundBuffer("TestBuffer", "assets/Audio/test.ogg");
	loadAudioSource("TestSound", "TestBuffer");
}

void AudioManager::unloadResources() { clear(); }

void AudioManager::loadSoundBuffer(const std::string& soundName, const std::string& soundPath)
{
	ALuint buffer = OpenALUtil::createSoundBuffer(soundPath);
	if (buffer != 0)
	{
		mAudioBufferMap[soundName] = buffer;
		std::cout << "AudioManager::loadSoundBuffer - Loaded sound: " << soundName << std::endl;
	}
	else
	{
		std::cerr << "AudioManager::loadSoundBuffer - Failed to load sound: " << soundName << std::endl;
	}
}

ALuint AudioManager::getSoundBuffer(const std::string& soundName)
{
	auto it = mAudioBufferMap.find(soundName);
	if (it != mAudioBufferMap.end())
	{
		return it->second;
	}
	else
	{
		std::cerr << "AudioManager::getSoundBuffer - Sound not found: " << soundName << std::endl;
		return 0;
	}
}

void AudioManager::loadAudioSource(const std::string& sourceName, const std::string& soundName)
{
	
	ALuint buffer = getSoundBuffer(soundName);
	if (buffer == 0)
	{
		std::cerr << "AudioManager::loadAudioSource - Invalid buffer for sound: " << soundName << std::endl;
		return;
	}

	auto audioSource = std::make_shared<AudioSource>(buffer);
	mAudioSourceMap[sourceName] = audioSource;
	std::cout << "AudioManager::loadAudioSource - Loaded audio source: " << sourceName << std::endl;
}

std::shared_ptr<AudioSource> AudioManager::getAudioSource(const std::string& sourceName)
{
	return mAudioSourceMap.at(sourceName);
}

void AudioManager::clear()
{
	// Stop and delete all sources
	for (auto& pair : mAudioSourceMap)
	{
		pair.second->stop();
		//pair.second->setBuffer(0);
	}
	mAudioSourceMap.clear();

	// Delete all buffers
	for (auto& pair : mAudioBufferMap)
	{
		OpenALUtil::deleteSoundBuffer(pair.second);
	}
	mAudioBufferMap.clear();
}
