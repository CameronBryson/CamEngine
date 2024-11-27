#pragma once
#include <AL/al.h>
#include <glm/vec3.hpp>
class AudioSource
{
public:
	AudioSource(ALuint buffer);
	~AudioSource();
	void play();
	void pause();
	void stop();
	void setSourcePitch(float pitch);
	void setSourceGain(float gain);
	void setSourceMinGain(float minGain);
	void setSourceMaxGain(float maxGain);
	void setSourceMaxDistance(float maxDistance);
	void setSourceRolloffFactor(float rolloffFactor);
	void setSourcePosition(const glm::vec3& position);
	void setSourceVelocity(const glm::vec3& velocity);
	void setSourceDirection(const glm::vec3& direction);
	void setSourceLoop(bool loop);
	void setBuffer(ALuint buffer);
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