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
	static ALuint createSource();
	static void deleteSource(ALuint source);
	static void playSoundSource(ALuint source);
	static void pauseSoundSource(ALuint source);
	static void stopSoundSource(ALuint source);
	static void setSourcePitch(ALuint source, float pitch);
	static void setSourceGain(ALuint source, float gain);
	static void setSourceMinGain(ALuint source, float min_gain);
	static void setSourceMaxGain(ALuint source, float max_gain);
	static void setSourceMaxDistance(ALuint source, float max_distance);
	static void setSourceRolloffFactor(ALuint source, float rolloff_factor);
	static void setSourcePosition(ALuint source, const glm::vec3& position);
	static void setSourceVelocity(ALuint source, const glm::vec3& velocity);
	static void setSourceDirection(ALuint source, const glm::vec3& direction);
	static void setSourceLoop(ALuint source, bool loop);
	static void setSourceBuffer(ALuint source, ALuint buffer);
	static void setListenerGain(float gain);
	static void setListenerPosition(const glm::vec3& position);
	static void setListenerVelocity(const glm::vec3& velocity);
	static void setListenerOrientation(const glm::vec3& at, const glm::vec3& up);
	static bool isBufferPlaying(ALuint source);

};
