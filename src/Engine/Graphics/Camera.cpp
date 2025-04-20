#include "pch.hpp"
#include "Camera.hpp"

#include "Engine/Util/GameSettings.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up,
			   float yaw, float pitch) : mFront(glm::vec3(0.0f, 0.0f, -1.0f))
{
	mPosition = std::move(position);
	mWorldUp = std::move(up);
	mYaw = yaw;
	mPitch = pitch;
	mFov = FOV;
	updateCamera();
	mProjectionMatrix = glm::perspective(glm::radians(mFov), static_cast<float>(settings::window_width) / static_cast<float>(settings::window_height), 0.1f, 10000.0f);
	mViewMatrix = glm::lookAt(mPosition, mPosition + mFront, mUp);
	mInverseProjectionMatrix = glm::inverse(mProjectionMatrix);

	mPreviousProjectionMatrix = mProjectionMatrix;
	mPreviousViewMatrix = mViewMatrix;
}
glm::mat4& Camera::getViewMatrix()
{
	return mViewMatrix;
}
glm::mat4& Camera::getProjectionMatrix()
{
	return mProjectionMatrix;
}
glm::mat4& Camera::getViewProjectionMatrix()
{
	return mViewProjectionMatrix;
}
glm::mat4& Camera::getInverseViewMatrix()
{
	return mInverseViewMatrix;
}
glm::mat4& Camera::getInverseProjectionMatrix()
{
	return mInverseProjectionMatrix;
}

glm::mat4& Camera::getInverseViewProjectionMatrix()
{
	return mInverseViewProjectionMatrix;
}

glm::mat4& Camera::getPreviousViewMatrix()
{
	return mPreviousViewMatrix;
}

glm::mat4& Camera::getPreviousProjectionMatrix()
{
	return mPreviousProjectionMatrix;
}

glm::mat4& Camera::getPreviousViewProjectionMatrix()
{
	return mPreviousViewProjectionMatrix;
}

void Camera::updateCamera()
{
	// calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	front.y = sin(glm::radians(mPitch));
	front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	mFront = glm::normalize(front);
	// also re-calculate the Right and Up vector
	mRight = glm::normalize(glm::cross(mFront, mWorldUp));
	// normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	mUp = glm::normalize(glm::cross(mRight, mFront));
	mViewMatrix = glm::lookAt(mPosition, mPosition + mFront, mUp);
	mProjectionMatrix = glm::perspective(glm::radians(mFov), settings::aspect_ratio, 0.1f, 10000.0f);
	mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
	mInverseViewMatrix = glm::inverse(mViewMatrix);
	mInverseProjectionMatrix = glm::inverse(mProjectionMatrix);
	mInverseViewProjectionMatrix = glm::inverse(mViewProjectionMatrix);

}


void Camera::storePreviousMatrices()
{
	mPreviousProjectionMatrix = mProjectionMatrix;
	mPreviousViewMatrix = mViewMatrix;
	mPreviousViewProjectionMatrix = mViewProjectionMatrix;
}

std::array<glm::vec4, 6> Camera::getFrustumPlanes()
{
	std::array<glm::vec4, 6> planes;

	// Get view-projection matrix
	glm::mat4 VP = mProjectionMatrix * mViewMatrix;

	// Helper lambda to normalize a plane
	auto normalizePlane = [](const glm::vec4& plane) -> glm::vec4
		{
			float magnitude = glm::length(glm::vec3(plane));
			return plane / magnitude;
		};

	// Since glm is column-major, extract each row:
	// Row i is: { VP[0][i], VP[1][i], VP[2][i], VP[3][i] }
	// Left Plane:  row3 + row0
	planes[0] = normalizePlane(glm::vec4(
		VP[0][3] + VP[0][0],
		VP[1][3] + VP[1][0],
		VP[2][3] + VP[2][0],
		VP[3][3] + VP[3][0]
	));
	// Right Plane: row3 - row0
	planes[1] = normalizePlane(glm::vec4(
		VP[0][3] - VP[0][0],
		VP[1][3] - VP[1][0],
		VP[2][3] - VP[2][0],
		VP[3][3] - VP[3][0]
	));
	// Bottom Plane: row3 + row1
	planes[2] = normalizePlane(glm::vec4(
		VP[0][3] + VP[0][1],
		VP[1][3] + VP[1][1],
		VP[2][3] + VP[2][1],
		VP[3][3] + VP[3][1]
	));
	// Top Plane: row3 - row1
	planes[3] = normalizePlane(glm::vec4(
		VP[0][3] - VP[0][1],
		VP[1][3] - VP[1][1],
		VP[2][3] - VP[2][1],
		VP[3][3] - VP[3][1]
	));
	// Near Plane: row3 + row2
	planes[4] = normalizePlane(glm::vec4(
		VP[0][3] + VP[0][2],
		VP[1][3] + VP[1][2],
		VP[2][3] + VP[2][2],
		VP[3][3] + VP[3][2]
	));
	// Far Plane: row3 - row2
	planes[5] = normalizePlane(glm::vec4(
		VP[0][3] - VP[0][2],
		VP[1][3] - VP[1][2],
		VP[2][3] - VP[2][2],
		VP[3][3] - VP[3][2]
	));

	return planes;
}


bool Camera::isSphereInFrustum(const glm::vec3& center, float radius)
{
	auto planes = getFrustumPlanes();

	// Test near and far planes first as they're most likely to cull
	float d = glm::dot(glm::vec3(planes[4]), center) + planes[4].w;
	if (d < -radius)
		return false;

	d = glm::dot(glm::vec3(planes[5]), center) + planes[5].w;
	if (d < -radius)
		return false;

	// Then test side planes
	for (int i = 0; i < 4; ++i)
	{
		d = glm::dot(glm::vec3(planes[i]), center) + planes[i].w;
		if (d < -radius)
			return false;
	}

	return true;
}

