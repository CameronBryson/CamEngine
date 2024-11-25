#pragma once

#include <string>
#include <AL/al.h>
class AudioBuffer
{
public:
	explicit AudioBuffer(const std::string& path);
	~AudioBuffer();

	ALuint getBuffer() const { return mBufferID; }
private:
	ALuint mBufferID = 0;

	void loadWav(const std::string& path);
};