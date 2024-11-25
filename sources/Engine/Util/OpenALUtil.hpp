#pragma once
#include <string>
#include <vector>
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
	static bool isBufferPlaying(ALuint source);
	static bool loadOggFile(const std::string& file, std::vector<char>& buffer, ALenum& format, ALsizei& freq);

};
