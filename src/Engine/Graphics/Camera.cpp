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
	       updateCamera();
	       projection_matrix = glm::perspective(glm::radians(Fov), static_cast<float>(settings::window_width) / static_cast<float>(settings::window_height), 0.1f, 10000.0f);
	       view_matrix = glm::lookAt(Position, Position + Front, Up);
		   inverse_projection_matrix = glm::inverse(projection_matrix);

		   previous_projection_matrix = projection_matrix;
		   previous_view_matrix = view_matrix;
	   }
glm::mat4& Camera::GetViewMatrix()
{
    return view_matrix;
}
glm::mat4& Camera::GetProjectionMatrix()
{
    return projection_matrix;
}
glm::mat4& Camera::GetViewProjectionMatrix()
{
	return view_projection_matrix;
}
glm::mat4& Camera::GetInverseViewMatrix()
{
	return inverse_view_matrix;
}
glm::mat4& Camera::GetInverseProjectionMatrix()
{
   return inverse_projection_matrix;
}

glm::mat4& Camera::GetInverseViewProjectionMatrix()
{
	return inverse_view_projection_matrix;
}

glm::mat4& Camera::GetPreviousViewMatrix()
{
	return previous_view_matrix;
}

glm::mat4& Camera::GetPreviousProjectionMatrix()
{
	return previous_projection_matrix;
}

glm::mat4& Camera::GetPreviousViewProjectionMatrix()
{
	return previous_view_projection_matrix;
}

void Camera::updateCamera()
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
	view_matrix = glm::lookAt(Position, Position + Front, Up);
	projection_matrix = glm::perspective(glm::radians(Fov), settings::aspect_ratio, 0.1f, 10000.0f);
	view_projection_matrix = projection_matrix * view_matrix;
	inverse_view_matrix = glm::inverse(view_matrix);
	inverse_projection_matrix = glm::inverse(projection_matrix);
	inverse_view_projection_matrix = glm::inverse(view_projection_matrix);

}


void Camera::storePreviousMatrices()
{
	previous_projection_matrix = projection_matrix;
	previous_view_matrix = view_matrix;
	previous_view_projection_matrix = view_projection_matrix;
}

std::array<glm::vec4, 6> Camera::GetFrustumPlanes()
{
    std::array<glm::vec4, 6> planes;

    // Get view-projection matrix
    glm::mat4 VP = projection_matrix * view_matrix;

    // Helper lambda to normalize a plane
    auto normalizePlane = [](const glm::vec4& plane) -> glm::vec4 {
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

