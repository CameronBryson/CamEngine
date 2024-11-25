#pragma once
#include <glm/vec3.hpp>
class AudioListener
{
public:
	void updateListener();
private:
	glm::vec3 mPosition = {0,0,0};
	glm::vec3 mVelocity = {0,0,0};
	glm::vec3 mOrientation = { 0,0,0 };

};
