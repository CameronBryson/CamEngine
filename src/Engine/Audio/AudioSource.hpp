#pragma once
#include <AL/al.h>
#include <glm/vec3.hpp>
class AudioSource
{
public:
	AudioSource(ALuint buffer);
	~AudioSource();
	void play() const;
	void pause();
	void stop() const;
	void setSourcePitch(float pitch) const;
	void setSourceGain(float gain) const;
	void setSourceMinGain(float minGain) const;
	void setSourceMaxGain(float maxGain) const;
	void setSourceMaxDistance(float maxDistance) const;
	void setSourceRolloffFactor(float rolloffFactor) const;
	void setSourcePosition(const glm::vec3& position) const;
	void setSourceVelocity(const glm::vec3& velocity) const;
	void setSourceDirection(const glm::vec3& direction) const;
	void setSourceLoop(bool loop) const;
	void setBuffer(ALuint buffer) const;
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