#pragma once
#include <glm/vec3.hpp>
class AudioListener
{
public:
	void setPosition(const glm::vec3& position);
	void setVelocity(const glm::vec3& velocity);
	void setOrientation(const glm::vec3& at, const glm::vec3& up);
	void updateListener();
private:
	glm::vec3 mPosition = {0,0,0};
	glm::vec3 mVelocity = {0,0,0};
	glm::vec3 mOrientation = { 0,0,0 };

};
