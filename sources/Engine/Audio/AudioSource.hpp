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
	void resume();

	void setBuffer(const ALuint buffer);
	void setPosition(const glm::vec3& position);
	void setVelocity(const glm::vec3& velocity);
	void setPitch(float pitch);
	void setGain(float gain);
	void setLooping(bool loop);

	ALuint getBuffer() const;
	bool isPlaying() const;


private:
	ALuint mBuffer;
	
};