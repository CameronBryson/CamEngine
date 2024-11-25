#pragma once
#include <string>
#include <AL/al.h>

#include "glm/vec3.hpp"

class OpenALUtil
{
public:
	static void init();
	static void shutdown();
	static ALuint createSoundBuffer(const std::string& soundPath);
	static void deleteSoundBuffer(ALuint buffer);
	static void playSound(ALuint buffer, float volume = 1.0f, bool loop = false);
	static void stopSound(ALuint buffer);
	static void setListenerPosition(const glm::vec3& position);
	static void setListenerVelocity(const glm::vec3& velocity);
	static void setListenerOrientation(const glm::vec3& at, const glm::vec3& up);

	static bool isBufferPlaying(ALuint source);

};
