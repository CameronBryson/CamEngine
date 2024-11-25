#include "AudioListener.hpp"
#include "Engine/Util/OpenALUtil.hpp"
void AudioListener::updateListener()
{
	OpenALUtil::setListenerPosition(mPosition);
	OpenALUtil::setListenerVelocity(mVelocity);
	OpenALUtil::setListenerOrientation(mOrientation, glm::vec3(0.0f, 1.0f, 0.0f));
}
