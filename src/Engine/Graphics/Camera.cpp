#include "pch.hpp"
#include "Camera.hpp"

#include "Engine/Util/GameSettings.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up,
               float yaw , float pitch ) : Front(glm::vec3(0.0f, 0.0f, -1.0f))
	   {
	       Position = position;
	       WorldUp = up;
	       Yaw = yaw;
	       Pitch = pitch;
		   Fov = FOV;
	       updateCameraVectors();
	       projection_matrix = glm::perspective(glm::radians(Fov), static_cast<float>(settings::window_width) / static_cast<float>(settings::window_height), 0.1f, 10000.0f);
	       view_matrix = glm::lookAt(Position, Position + Front, Up);
		   inverse_projection_matrix = glm::inverse(projection_matrix);
	   }
glm::mat4& Camera::GetViewMatrix()
{
    view_matrix = glm::lookAt(Position, Position + Front, Up);
    return view_matrix;
}
glm::mat4& Camera::GetProjectionMatrix()
{
    return projection_matrix;
}
glm::mat4& Camera::GetInverseProjectionMatrix()
{
   return inverse_projection_matrix;
}

void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));
    // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::updateProjectionMatrix()
{
   projection_matrix = glm::perspective(glm::radians(Fov), static_cast<float>(settings::window_width) / static_cast<float>(settings::window_height), 0.1f, 10000.0f);
   inverse_projection_matrix = glm::inverse(projection_matrix);
}

std::array<glm::vec4, 6> Camera::GetFrustumPlanes()
{
    std::array<glm::vec4, 6> planes;

    // Get view-projection matrix
    glm::mat4 VP = projection_matrix * view_matrix;

    // Left plane
    planes[0].x = VP[0][3] + VP[0][0];
    planes[0].y = VP[1][3] + VP[1][0];
    planes[0].z = VP[2][3] + VP[2][0];
    planes[0].w = VP[3][3] + VP[3][0];

    // Right plane
    planes[1].x = VP[0][3] - VP[0][0];
    planes[1].y = VP[1][3] - VP[1][0];
    planes[1].z = VP[2][3] - VP[2][0];
    planes[1].w = VP[3][3] - VP[3][0];

    // Bottom plane
    planes[2].x = VP[0][3] + VP[0][1];
    planes[2].y = VP[1][3] + VP[1][1];
    planes[2].z = VP[2][3] + VP[2][1];
    planes[2].w = VP[3][3] + VP[3][1];

    // Top plane
    planes[3].x = VP[0][3] - VP[0][1];
    planes[3].y = VP[1][3] - VP[1][1];
    planes[3].z = VP[2][3] - VP[2][1];
    planes[3].w = VP[3][3] - VP[3][1];

    // Near plane
    planes[4].x = VP[0][2];
    planes[4].y = VP[1][2];
    planes[4].z = VP[2][2];
    planes[4].w = VP[3][2];

    // Far plane
    planes[5].x = VP[0][3] - VP[0][2];
    planes[5].y = VP[1][3] - VP[1][2];
    planes[5].z = VP[2][3] - VP[2][2];
    planes[5].w = VP[3][3] - VP[3][2];

    // Normalize all planes
    for (auto& plane : planes)
    {
        float length = glm::length(glm::vec3(plane));
        if (length > 0.0f)
            plane /= length;
    }

    return planes;
}

bool Camera::isSphereInFrustum(const glm::vec3& center, float radius)
{
    auto planes = GetFrustumPlanes();

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

