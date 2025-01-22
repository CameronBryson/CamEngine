#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <AL/al.h>

#include "Engine/Audio/AudioListener.hpp"
class AudioSource;
class AudioManager
{
public:
	AudioManager() = default;
	~AudioManager();
	void loadResources();
	void unloadResources();
	void loadSoundBuffer(const std::string& soundName, const std::string& soundPath);
	ALuint getSoundBuffer(const std::string& soundName);

	void loadAudioSource(const std::string& sourceName, const std::string& soundName);
	std::shared_ptr<AudioSource> getAudioSource(const std::string& sourceName);

	void clear();
	

private:
	std::unordered_map<std::string, ALuint> mAudioBufferMap;
	std::unordered_map<std::string, std::shared_ptr<AudioSource>> mAudioSourceMap;
	AudioListener mAudioListener;
};