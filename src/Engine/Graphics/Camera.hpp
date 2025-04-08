#pragma once
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "Engine/Components.hpp"
#include <array>

// Default camera values
constexpr float YAW = -90.0f;
constexpr float PITCH = 0.0f;
constexpr float FOV = 90.0f;


class Camera
{
public:
	glm::vec3 mPosition;
	glm::vec3 mFront;
	glm::vec3 mUp;
	glm::vec3 mRight;
	glm::vec3 mWorldUp;
	float mYaw;
	float mPitch;
	float mFov;
	glm::mat4 mViewMatrix;
	glm::mat4 mProjectionMatrix;
	glm::mat4 mViewProjectionMatrix;
	glm::mat4 mInverseViewMatrix;
	glm::mat4 mInverseProjectionMatrix;
	glm::mat4 mInverseViewProjectionMatrix;

	glm::mat4 mPreviousViewMatrix;
	glm::mat4 mPreviousProjectionMatrix;
	glm::mat4 mPreviousViewProjectionMatrix;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

	glm::mat4& getViewMatrix();

	glm::mat4& getProjectionMatrix();

	glm::mat4& getViewProjectionMatrix();

	glm::mat4& getInverseViewMatrix();
	glm::mat4& getInverseProjectionMatrix();
	glm::mat4& getInverseViewProjectionMatrix();

	glm::mat4& getPreviousViewMatrix();

	glm::mat4& getPreviousProjectionMatrix();

	glm::mat4& getPreviousViewProjectionMatrix();

	std::array<glm::vec4, 6> getFrustumPlanes();


	void updateCamera();
	void storePreviousMatrices();

	bool isSphereInFrustum(const glm::vec3& center, float radius);
};