#pragma once
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "Engine/Components.hpp"
#include <array>

/// Default camera values
constexpr float YAW = -90.0f;
constexpr float PITCH = 0.0f;
constexpr float FOV = 90.0f;

/**
 * @class Camera
 * @brief Represents a 3D camera in the scene
 *
 * Handles view and projection matrices, camera movement, and frustum calculations
 * for rendering and culling operations.
 */
class Camera
{
public:
	/**
	 * @brief Construct a camera with specified parameters
	 * @param position Initial camera position
	 * @param up World up vector
	 * @param yaw Initial yaw angle in degrees
	 * @param pitch Initial pitch angle in degrees
	 */
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		   glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		   float yaw = YAW,
		   float pitch = PITCH);

	/**
	 * @brief Get the current view matrix
	 * @return Reference to the view matrix
	 */
	glm::mat4& getViewMatrix();

	/**
	 * @brief Get the current projection matrix
	 * @return Reference to the projection matrix
	 */
	glm::mat4& getProjectionMatrix();

	/**
	 * @brief Get the combined view-projection matrix
	 * @return Reference to the view-projection matrix
	 */
	glm::mat4& getViewProjectionMatrix();

	/**
	 * @brief Get the inverse of the view matrix
	 * @return Reference to the inverse view matrix
	 */
	glm::mat4& getInverseViewMatrix();

	/**
	 * @brief Get the inverse of the projection matrix
	 * @return Reference to the inverse projection matrix
	 */
	glm::mat4& getInverseProjectionMatrix();

	/**
	 * @brief Get the inverse of the view-projection matrix
	 * @return Reference to the inverse view-projection matrix
	 */
	glm::mat4& getInverseViewProjectionMatrix();

	/**
	 * @brief Get the previous frame's view matrix
	 * @return Reference to the previous view matrix
	 */
	glm::mat4& getPreviousViewMatrix();

	/**
	 * @brief Get the previous frame's projection matrix
	 * @return Reference to the previous projection matrix
	 */
	glm::mat4& getPreviousProjectionMatrix();

	/**
	 * @brief Get the previous frame's view-projection matrix
	 * @return Reference to the previous view-projection matrix
	 */
	glm::mat4& getPreviousViewProjectionMatrix();

	/**
	 * @brief Calculate the frustum planes for culling
	 * @return Array of 6 planes as vec4 (left, right, bottom, top, near, far)
	 */
	std::array<glm::vec4, 6> getFrustumPlanes() const;

	/**
	 * @brief Update camera matrices based on current position and orientation
	 */
	void updateCamera();

	/**
	 * @brief Store current matrices as previous matrices for temporal effects
	 */
	void storePreviousMatrices();

	/**
	 * @brief Test if a sphere is visible within the camera frustum
	 * @param center The center position of the sphere
	 * @param radius The radius of the sphere
	 * @return True if the sphere is at least partially visible
	 */
	bool isSphereInFrustum(const glm::vec3& center, float radius) const;

public:
	glm::vec3 mPosition;          ///< Camera position in world space
	glm::vec3 mFront;             ///< Camera front/looking direction (normalized)
	glm::vec3 mUp;                ///< Camera up vector
	glm::vec3 mRight;             ///< Camera right vector
	glm::vec3 mWorldUp;           ///< Global up vector
	float mYaw;                   ///< Yaw angle in degrees
	float mPitch;                 ///< Pitch angle in degrees
	float mFov;                   ///< Field of view in degrees

	// Current frame matrices
	glm::mat4 mViewMatrix;                  ///< View matrix for current frame
	glm::mat4 mProjectionMatrix;            ///< Projection matrix for current frame
	glm::mat4 mViewProjectionMatrix;        ///< Combined view-projection matrix
	glm::mat4 mInverseViewMatrix;           ///< Inverse view matrix
	glm::mat4 mInverseProjectionMatrix;     ///< Inverse projection matrix
	glm::mat4 mInverseViewProjectionMatrix; ///< Inverse view-projection matrix

	// Previous frame matrices for temporal techniques
	glm::mat4 mPreviousViewMatrix;          ///< View matrix from previous frame
	glm::mat4 mPreviousProjectionMatrix;    ///< Projection matrix from previous frame
	glm::mat4 mPreviousViewProjectionMatrix;///< View-projection matrix from previous frame
};