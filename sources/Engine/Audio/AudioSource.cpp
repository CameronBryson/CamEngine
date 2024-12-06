#include "Engine/pch.hpp"
#include "AudioSource.hpp"

#include <iostream>
#include <ostream>

#include "Engine/Util/OpenALUtil.hpp"

AudioSource::AudioSource(ALuint buffer) : mBuffer(buffer), mSource(0)
{
	mSource = OpenALUtil::createSource();
	updateSource();
}

AudioSource::~AudioSource()
{
	stop();
	setBuffer(0);
	//OpenALUtil::deleteSoundBuffer(mBuffer);
	OpenALUtil::deleteSource(mSource);
}

void AudioSource::play() const
{
	alSourcePlay(mSource);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::playSoundSource - Failed to play source ID: " << mSource << std::endl;
	}
	else
	{
		std::cout << "OpenALUtil::playSoundSource - Playing source ID: " << mSource << std::endl;
	}
}

void AudioSource::pause()
{
	alSourcePause(mSource);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::pauseSoundSource - Failed to pause source ID: " << mSource << std::endl;
	}
	else
	{
		std::cout << "OpenALUtil::pauseSoundSource - Paused source ID: " << mSource << std::endl;
	}
}

void AudioSource::stop() const
{
	alSourceStop(mSource);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::stopSoundSource - Failed to stop source ID: " << mSource << std::endl;
	}
	else
	{
		std::cout << "OpenALUtil::stopSoundSource - Stopped source ID: " << mSource << std::endl;
	}
}

void AudioSource::setSourcePitch(float pitch) const
{
	alSourcef(mSource, AL_PITCH, pitch);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::setSourcePitch - Failed to set pitch for source ID: " << mSource << std::endl;
	}
	else
	{
		std::cout << "OpenALUtil::setSourcePitch - Set pitch to " << pitch << " for source ID: " << mSource << std::endl;
	}
}

void AudioSource::setSourceGain(float gain) const
{
	alSourcef(mSource, AL_GAIN, gain);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::setSourceGain - Failed to set gain for source ID: " << mSource << std::endl;
	}
	else
	{
		std::cout << "OpenALUtil::setSourceGain - Set gain to " << gain << " for source ID: " << mSource << std::endl;
	}
}

void AudioSource::setSourceMinGain(float minGain) const
{
	alSourcef(mSource, AL_MIN_GAIN, minGain);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::setSourceMinGain - Failed to set minimum gain for source ID: " << mSource << std::endl;
	}
	else
	{
		std::cout << "OpenALUtil::setSourceMinGain - Set minimum gain to " << minGain << " for source ID: " << mSource << std::endl;
	}
}

void AudioSource::setSourceMaxGain(float maxGain) const
{
	alSourcef(mSource, AL_MAX_GAIN, maxGain);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::setSourceMaxGain - Failed to set maximum gain for source ID: " << mSource << std::endl;
	}
	else
	{
		std::cout << "OpenALUtil::setSourceMaxGain - Set maximum gain to " << maxGain << " for source ID: " << mSource << std::endl;
	}
}

void AudioSource::setSourceMaxDistance(float maxDistance) const
{
	alSourcef(mSource, AL_MAX_DISTANCE, maxDistance);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::setSourceMaxDistance - Failed to set max distance for source ID: " << mSource << std::endl;
	}
	else
	{
		std::cout << "OpenALUtil::setSourceMaxDistance - Set max distance to " << maxDistance << " for source ID: " << mSource << std::endl;
	}
}

void AudioSource::setSourceRolloffFactor(float rolloffFactor) const
{
	alSourcef(mSource, AL_ROLLOFF_FACTOR, rolloffFactor);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::setSourceRolloffFactor - Failed to set rolloff factor for source ID: " << mSource << std::endl;
	}
	else
	{
		std::cout << "OpenALUtil::setSourceRolloffFactor - Set rolloff factor to " << rolloffFactor << " for source ID: " << mSource << std::endl;
	}
}

void AudioSource::setSourcePosition(const glm::vec3& position) const
{
	alSource3f(mSource, AL_POSITION, position.x, position.y, position.z);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::setSourcePosition - Failed to set position for source ID: " << mSource << std::endl;
	}
	else
	{
		std::cout << "OpenALUtil::setSourcePosition - Set position to (" << position.x << ", " << position.y << ", " << position.z << ") for source ID: " << mSource << std::endl;
	}
}

void AudioSource::setSourceVelocity(const glm::vec3& velocity) const
{
	alSource3f(mSource, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::setSourceVelocity - Failed to set velocity for source ID: " << mSource << std::endl;
	}
	else
	{
		std::cout << "OpenALUtil::setSourceVelocity - Set velocity to (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ") for source ID: " << mSource << std::endl;
	}
}

void AudioSource::setSourceDirection(const glm::vec3& direction) const
{
	alSource3f(mSource, AL_DIRECTION, direction.x, direction.y, direction.z);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::setSourceDirection - Failed to set direction for source ID: " << mSource << std::endl;
	}
	else
	{
		std::cout << "OpenALUtil::setSourceDirection - Set direction to (" << direction.x << ", " << direction.y << ", " << direction.z << ") for source ID: " << mSource << std::endl;
	}
}

void AudioSource::setSourceLoop(bool loop) const
{
	alSourcei(mSource, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::setSourceLoop - Failed to set looping for source ID: " << mSource << std::endl;
	}
	else
	{
		std::cout << "OpenALUtil::setSourceLoop - Set looping to " << (loop ? "true" : "false") << " for source ID: " << mSource << std::endl;
	}
}

void AudioSource::setBuffer(ALuint buffer) const
{
	alSourcei(mSource, AL_BUFFER, buffer);
	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::setSourceBuffer - Failed to set buffer for source ID: " << mSource << std::endl;
	}
	else
	{
		std::cout << "OpenALUtil::setmSourceBuffer - Set buffer ID " << buffer << " for mSource ID: " << mSource << std::endl;
	}
}


void AudioSource::updateSource()
{
	setBuffer(mBuffer);
	setSourcePitch(mPitch);
	setSourceGain(mGain);
	setSourceMinGain(mMinGain);
	setSourceMaxGain(mMaxGain);
	setSourceMaxDistance(mMaxDistance);
	setSourceRolloffFactor(mRolloffFactor);
	setSourcePosition(mPosition);
	setSourceVelocity(mVelocity);
	setSourceDirection(mDirection);
	setSourceLoop(mLoop);

}


ALuint AudioSource::getBuffer() const
{
	return mBuffer;
}

bool AudioSource::isPlaying() const
{
	return OpenALUtil::isBufferPlaying(mBuffer);
}
