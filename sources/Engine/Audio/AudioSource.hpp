#pragma once
#include <AL/al.h>
#include <glm/vec3.hpp>
class AudioSource
{
public:
	AudioSource();
	~AudioSource() = default;
	void play();
	void pause();
	void stop();

	void updateSource();
	ALuint getBuffer() const;
	bool isPlaying() const;


private:
	ALuint mSource;
	ALuint mBuffer;
	float mPitch = 1.0f;
	float mGain = 1.0f;
	float mMinGain = 1.0f;
	float mMaxGain = 1.0f;
	float mMaxDistance = 1.0f;
	float mRolloffFactor = 1.0f;
	glm::vec3 mPosition;
	glm::vec3 mVelocity;
	glm::vec3 mDirection;
	bool mLoop = false;
	
};