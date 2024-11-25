#include "AudioListener.hpp"

#include <iostream>
#include <ostream>

#include "Engine/Util/OpenALUtil.hpp"

void AudioListener::setPosition(const glm::vec3& position)
{
	alListener3f(AL_POSITION, position.x, position.y, position.z);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::setListenerPosition - Failed to set listener position." << std::endl;
	}
	else
	{
		std::cout << "OpenALUtil::setListenerPosition - Set listener position to (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
	}
}

void AudioListener::setVelocity(const glm::vec3& velocity)
{
	alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::setListenerVelocity - Failed to set listener velocity." << std::endl;
	}
	else
	{
		std::cout << "OpenALUtil::setListenerVelocity - Set listener velocity to (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;
	}
}

void AudioListener::setOrientation(const glm::vec3& at, const glm::vec3& up)
{
	float orientation[] = { at.x, at.y, at.z, up.x, up.y, up.z };
	alListenerfv(AL_ORIENTATION, orientation);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::setListenerOrientation - Failed to set listener orientation." << std::endl;
	}
	else
	{
		std::cout << "OpenALUtil::setListenerOrientation - Set listener orientation." << std::endl;
	}
}

void AudioListener::updateListener()
{
	setPosition(mPosition);
	setVelocity(mVelocity);
	setOrientation(mOrientation, glm::vec3(0.0f, 1.0f, 0.0f));
}
