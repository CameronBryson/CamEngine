#pragma once
#include <string>
#include <unordered_map>
#include "Engine/Audio/AudioSource.hpp"
class AudioManager
{
public:
	AudioManager() = default;
	void loadSoundBuffer(const std::string& soundName, const std::string& soundPath);
	ALuint getSoundBuffer(const std::string& soundName);

	void loadAudioSource(const std::string& sourceName, const std::string& soundName);
	AudioSource& getAudioSource(const std::string& sourceName);

	void clear();
	

private:
	std::unordered_map<std::string, ALuint> mAudioBufferMap;
	std::unordered_map<std::string, AudioSource> mAudioSourceMap;
};