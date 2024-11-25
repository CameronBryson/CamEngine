#pragma once
#include <string>
#include <unordered_map>
class AudioManager
{
public:
	AudioManager() = default;
	unsigned int LoadSound(const std::string& soundName, const std::string& soundPath);
	void PlaySound(const std::string& soundName, float volume = 1.0f, bool looping = false);
	void StopSound(const std::string& soundName);
	

private:
	std::unordered_map<std::string, unsigned int> mSoundMap;
};