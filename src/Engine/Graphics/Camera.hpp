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


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float Fov;
    glm::mat4 projection_matrix;
    glm::mat4 view_matrix;
    glm::mat4 inverse_projection_matrix;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

    // constructor with scalar values
    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    [[nodiscard]] glm::mat4& GetViewMatrix();

    [[nodiscard]] glm::mat4& GetProjectionMatrix();

	[[nodiscard]] glm::mat4& GetInverseProjectionMatrix();

	[[nodiscard]] std::array<glm::vec4, 6> GetFrustumPlanes();


    // calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors();
	void updateProjectionMatrix();

	bool isSphereInFrustum(const glm::vec3& center, float radius);
};