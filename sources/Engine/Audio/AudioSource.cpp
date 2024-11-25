#include "AudioSource.hpp"
#include "Engine/Util/OpenALUtil.hpp"

AudioSource::AudioSource()
{
}

void AudioSource::play()
{
	OpenALUtil::playSoundSource(mSource);
}

void AudioSource::pause()
{
	OpenALUtil::pauseSoundSource(mSource);
}

void AudioSource::stop()
{
	OpenALUtil::stopSoundSource(mSource);
}


void AudioSource::updateSource()
{
	OpenALUtil::setSourceBuffer(mSource, mBuffer);
	OpenALUtil::setSourcePitch(mSource, mPitch);
	OpenALUtil::setSourceGain(mSource, mGain);
	OpenALUtil::setSourceMinGain(mSource, mMinGain);
	OpenALUtil::setSourceMaxGain(mSource, mMaxGain);
	OpenALUtil::setSourceMaxDistance(mSource, mMaxDistance);
	OpenALUtil::setSourceRolloffFactor(mSource, mRolloffFactor);
	OpenALUtil::setSourcePosition(mSource, mPosition);
	OpenALUtil::setSourceVelocity(mSource, mVelocity);
	OpenALUtil::setSourceDirection(mSource, mDirection);
	OpenALUtil::setSourceLoop(mSource, mLoop);
}


ALuint AudioSource::getBuffer() const
{
	return mBuffer;
}

bool AudioSource::isPlaying() const
{
	return OpenALUtil::isBufferPlaying(mBuffer);
}
