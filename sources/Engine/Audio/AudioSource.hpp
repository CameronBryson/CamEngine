#pragma once
#include <AL/al.h>
class AudioBuffer;
class AudioSource
{
public:
	AudioSource();
	~AudioSource();

private:
	ALuint mSourceID = 0;
};