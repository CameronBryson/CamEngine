#include "AudioManager.hpp"

void AudioManager::loadSoundBuffer(const std::string& soundName, const std::string& soundPath)
{
	
}

ALuint AudioManager::getSoundBuffer(const std::string& soundName)
{
	return mAudioBufferMap.at(soundName);
}

void AudioManager::loadAudioSource(const std::string& sourceName, const std::string& soundName)
{
	
}

AudioSource& AudioManager::getAudioSource(const std::string& sourceName)
{
	return mAudioSourceMap.at(sourceName);
}

void AudioManager::clear()
{
	mAudioBufferMap.clear();
	mAudioSourceMap.clear();
}
