#include "OpenALUtil.hpp"

#include <iostream>
#include <ostream>
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
    alDeleteBuffers(1, &buffer);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::deleteSoundBuffer - Error deleting OpenAL buffer." << std::endl;
    }
    else
    {
        std::cout << "OpenALUtil::deleteSoundBuffer - Deleted buffer ID: " << buffer << std::endl;
    }
}

ALuint OpenALUtil::createSource()
{
    ALuint source;
    alGenSources(1, &source);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::createSource - Error generating OpenAL source." << std::endl;
        return 0;
    }

    std::cout << "OpenALUtil::createSource - Generated source ID: " << source << std::endl;
    return source;
}

void OpenALUtil::deleteSource(ALuint source)
{
    alDeleteSources(1, &source);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::deleteSource - Error deleting OpenAL source." << std::endl;
    }
    else
    {
        std::cout << "OpenALUtil::deleteSource - Deleted source ID: " << source << std::endl;
    }
}

void OpenALUtil::playSoundSource(ALuint source)
{
    alSourcePlay(source);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::playSoundSource - Failed to play source ID: " << source << std::endl;
    }
    else
    {
        std::cout << "OpenALUtil::playSoundSource - Playing source ID: " << source << std::endl;
    }
}

void OpenALUtil::pauseSoundSource(ALuint source)
{
    alSourcePause(source);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::pauseSoundSource - Failed to pause source ID: " << source << std::endl;
    }
    else
    {
        std::cout << "OpenALUtil::pauseSoundSource - Paused source ID: " << source << std::endl;
    }
}

void OpenALUtil::stopSoundSource(ALuint source)
{
    alSourceStop(source);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::stopSoundSource - Failed to stop source ID: " << source << std::endl;
    }
    else
    {
        std::cout << "OpenALUtil::stopSoundSource - Stopped source ID: " << source << std::endl;
    }
}

void OpenALUtil::setSourcePitch(ALuint source, float pitch)
{
    alSourcef(source, AL_PITCH, pitch);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::setSourcePitch - Failed to set pitch for source ID: " << source << std::endl;
    }
    else
    {
        std::cout << "OpenALUtil::setSourcePitch - Set pitch to " << pitch << " for source ID: " << source << std::endl;
    }
}

void OpenALUtil::setSourceGain(ALuint source, float gain)
{
    alSourcef(source, AL_GAIN, gain);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::setSourceGain - Failed to set gain for source ID: " << source << std::endl;
    }
    else
    {
        std::cout << "OpenALUtil::setSourceGain - Set gain to " << gain << " for source ID: " << source << std::endl;
    }
}

void OpenALUtil::setSourceMinGain(ALuint source, float min_gain)
{
    alSourcef(source, AL_MIN_GAIN, min_gain);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::setSourceMinGain - Failed to set minimum gain for source ID: " << source << std::endl;
    }
    else
    {
        std::cout << "OpenALUtil::setSourceMinGain - Set minimum gain to " << min_gain << " for source ID: " << source << std::endl;
    }
}

void OpenALUtil::setSourceMaxGain(ALuint source, float max_gain)
{
    alSourcef(source, AL_MAX_GAIN, max_gain);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::setSourceMaxGain - Failed to set maximum gain for source ID: " << source << std::endl;
    }
    else
    {
        std::cout << "OpenALUtil::setSourceMaxGain - Set maximum gain to " << max_gain << " for source ID: " << source << std::endl;
    }
}

void OpenALUtil::setSourceMaxDistance(ALuint source, float max_distance)
{
    alSourcef(source, AL_MAX_DISTANCE, max_distance);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::setSourceMaxDistance - Failed to set max distance for source ID: " << source << std::endl;
    }
    else
    {
        std::cout << "OpenALUtil::setSourceMaxDistance - Set max distance to " << max_distance << " for source ID: " << source << std::endl;
    }
}

void OpenALUtil::setSourceRolloffFactor(ALuint source, float rolloff_factor)
{
    alSourcef(source, AL_ROLLOFF_FACTOR, rolloff_factor);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::setSourceRolloffFactor - Failed to set rolloff factor for source ID: " << source << std::endl;
    }
    else
    {
        std::cout << "OpenALUtil::setSourceRolloffFactor - Set rolloff factor to " << rolloff_factor << " for source ID: " << source << std::endl;
    }
}

void OpenALUtil::setSourcePosition(ALuint source, const glm::vec3& position)
{
    alSource3f(source, AL_POSITION, position.x, position.y, position.z);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::setSourcePosition - Failed to set position for source ID: " << source << std::endl;
    }
    else
    {
        std::cout << "OpenALUtil::setSourcePosition - Set position to (" << position.x << ", " << position.y << ", " << position.z << ") for source ID: " << source << std::endl;
    }
}

void OpenALUtil::setSourceVelocity(ALuint source, const glm::vec3& velocity)
{
    alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::setSourceVelocity - Failed to set velocity for source ID: " << source << std::endl;
    }
    else
    {
        std::cout << "OpenALUtil::setSourceVelocity - Set velocity to (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ") for source ID: " << source << std::endl;
    }
}

void OpenALUtil::setSourceDirection(ALuint source, const glm::vec3& direction)
{
    alSource3f(source, AL_DIRECTION, direction.x, direction.y, direction.z);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::setSourceDirection - Failed to set direction for source ID: " << source << std::endl;
    }
    else
    {
        std::cout << "OpenALUtil::setSourceDirection - Set direction to (" << direction.x << ", " << direction.y << ", " << direction.z << ") for source ID: " << source << std::endl;
    }
}

void OpenALUtil::setSourceLoop(ALuint source, bool loop)
{
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::setSourceLoop - Failed to set looping for source ID: " << source << std::endl;
    }
    else
    {
        std::cout << "OpenALUtil::setSourceLoop - Set looping to " << (loop ? "true" : "false") << " for source ID: " << source << std::endl;
    }
}

void OpenALUtil::setSourceBuffer(ALuint source, ALuint buffer)
{
    alSourcei(source, AL_BUFFER, buffer);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::setSourceBuffer - Failed to set buffer for source ID: " << source << std::endl;
    }
    else
    {
        std::cout << "OpenALUtil::setSourceBuffer - Set buffer ID " << buffer << " for source ID: " << source << std::endl;
    }
}

void OpenALUtil::setListenerGain(float gain)
{
    alListenerf(AL_GAIN, gain);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cerr << "OpenALUtil::setListenerGain - Failed to set listener gain." << std::endl;
    }
    else
    {
        std::cout << "OpenALUtil::setListenerGain - Set listener gain to " << gain << std::endl;
    }
}

void OpenALUtil::setListenerPosition(const glm::vec3& position)
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

void OpenALUtil::setListenerVelocity(const glm::vec3& velocity)
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

void OpenALUtil::setListenerOrientation(const glm::vec3& at, const glm::vec3& up)
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




bool OpenALUtil::isBufferPlaying(ALuint source)
{
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}
