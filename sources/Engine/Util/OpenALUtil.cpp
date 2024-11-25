#include "OpenALUtil.hpp"
#include <AL/alc.h>

void OpenALUtil::init()
{
}

void OpenALUtil::shutdown()
{
}

ALuint OpenALUtil::createSoundBuffer(const std::string& soundPath)
{
	return 1;
}

void OpenALUtil::deleteSoundBuffer(ALuint buffer)
{
}

void OpenALUtil::playSound(ALuint buffer, float volume, bool loop)
{
}

void OpenALUtil::stopSound(ALuint buffer)
{
}

void OpenALUtil::setListenerPosition(const glm::vec3& position)
{
}

void OpenALUtil::setListenerVelocity(const glm::vec3& velocity)
{
}

void OpenALUtil::setListenerOrientation(const glm::vec3& at, const glm::vec3& up)
{
}

bool OpenALUtil::isBufferPlaying(ALuint source)
{
	return false;
}
