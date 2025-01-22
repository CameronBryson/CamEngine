
#include "pch.hpp"
#include "OpenALUtil.hpp"

#include <iostream>
#include <ostream>
#include <vector>
#include <AL/alc.h>

#include "EngineUtil.hpp"
#include "stb_vorbis.c"

#include "Engine/Managers/GameManager.hpp"

void OpenALUtil::init()
{
	// Initialize OpenAL
	GameManager::mAudioDevice = alcOpenDevice(nullptr); // Open the default device
	if (!GameManager::mAudioDevice)
	{
		std::cerr << "OpenALUtil::init - Failed to open the default audio device." << std::endl;
		return;
	}
	else
	{
		std::cout << "OpenALUtil::init - Successfully opened the default audio device." << std::endl;
	}

	GameManager::mAudioContext = alcCreateContext(GameManager::mAudioDevice, nullptr);
	if (!GameManager::mAudioContext)
	{
		std::cerr << "OpenALUtil::init - Failed to create audio context." << std::endl;
		alcCloseDevice(GameManager::mAudioDevice);
		GameManager::mAudioDevice = nullptr;
		return;
	}
	else
	{
		std::cout << "OpenALUtil::init - Successfully created audio context." << std::endl;
	}

	if (!alcMakeContextCurrent(GameManager::mAudioContext))
	{
		std::cerr << "OpenALUtil::init - Failed to make audio context current." << std::endl;
		alcDestroyContext(GameManager::mAudioContext);
		alcCloseDevice(GameManager::mAudioDevice);
		GameManager::mAudioContext = nullptr;
		GameManager::mAudioDevice = nullptr;
		return;
	}
	else
	{
		std::cout << "OpenALUtil::init - Successfully made audio context current." << std::endl;
	}
}

void OpenALUtil::shutdown()
{
	alcMakeContextCurrent(nullptr);
	if (GameManager::mAudioContext)
	{
		alcDestroyContext(GameManager::mAudioContext);
		GameManager::mAudioContext = nullptr;
	}
	if (GameManager::mAudioDevice)
	{
		alcCloseDevice(GameManager::mAudioDevice);
		GameManager::mAudioDevice = nullptr;
	}
}

ALuint OpenALUtil::createSoundBuffer(const std::string& soundPath)
{
	ALuint buffer = 0;

	// Load audio data from file
	std::vector<char> audioData;
	ALenum format = 0;
	ALsizei freq = 0;

	if (!loadOggFile(soundPath, audioData, format, freq))
	{
		std::cerr << "OpenALUtil::createSoundBuffer - Failed to load audio file: " << soundPath << std::endl;
		return 0;
	}

	// Generate OpenAL buffer and load audio data into it
	alGenBuffers(1, &buffer);
	alBufferData(buffer, format, audioData.data(), static_cast<ALsizei>(audioData.size()), freq);

	if (alGetError() != AL_NO_ERROR)
	{
		std::cerr << "OpenALUtil::createSoundBuffer - Error creating OpenAL buffer for: " << soundPath << std::endl;
		if (buffer != 0)
		{
			alDeleteBuffers(1, &buffer);
		}
		return 0;
	}

	std::cout << "OpenALUtil::createSoundBuffer - Loaded buffer ID: " << buffer << " for file: " << soundPath << std::endl;
	return buffer;
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



bool OpenALUtil::isBufferPlaying(ALuint source)
{
	ALint state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	return state == AL_PLAYING;
}

bool OpenALUtil::loadOggFile(const std::string& filePath, std::vector<char>& audioData, ALenum& format, ALsizei& freq)
{
	int channels = 0;
	int sampleRate = 0;
	short* output = nullptr;
	int samples = stb_vorbis_decode_filename(engine_util::buildPath(filePath).c_str(), &channels, &sampleRate, &output);

	if (samples == -1)
	{
		std::cerr << "OpenALUtil::loadOggFile - Failed to open Ogg Vorbis file: " << filePath << std::endl;
		return false;
	}

	// Determine the format
	if (channels == 1)
		format = AL_FORMAT_MONO16;
	else if (channels == 2)
		format = AL_FORMAT_STEREO16;
	else
	{
		std::cerr << "OpenALUtil::loadOggFile - Unsupported number of channels: " << channels << std::endl;
		free(output);
		return false;
	}

	freq = sampleRate;
	audioData.assign(reinterpret_cast<char*>(output), reinterpret_cast<char*>(output) + (samples * channels * sizeof(short)));
	free(output);

	return true;
}
