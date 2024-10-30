#pragma once
#include "platform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GameSettings.hpp"
#include "Components.hpp"


// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float ZOOM = 45.0f;


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
    float Zoom;
    glm::mat4 projection_matrix;
    glm::mat4 view_matrix;
    c_transform * camera_follow_target_;
    c_transform * skybox_tranform;
    glm::vec3 follow_offset;
    double previous_mouse_x = 0;
    double previous_mouse_y = 0;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

    // constructor with scalar values
    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    [[nodiscard]] glm::mat4& GetViewMatrix();

    [[nodiscard]] glm::mat4& GetProjectionMatrix();


    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
};