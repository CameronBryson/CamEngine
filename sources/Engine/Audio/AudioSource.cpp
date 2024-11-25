#include "AudioSource.hpp"

AudioSource::AudioSource()
{
}

void AudioSource::play()
{
}

void AudioSource::pause()
{
}

void AudioSource::stop()
{
}

void AudioSource::resume()
{
}

void AudioSource::setBuffer(const ALuint buffer)
{
}

void AudioSource::setPosition(const glm::vec3& position)
{
}

void AudioSource::setVelocity(const glm::vec3& velocity)
{
}

void AudioSource::setPitch(float pitch)
{
}

void AudioSource::setGain(float gain)
{
}

void AudioSource::setLooping(bool loop)
{
}

ALuint AudioSource::getBuffer() const
{
	return 1;
}

bool AudioSource::isPlaying() const
{
	return false;
}
