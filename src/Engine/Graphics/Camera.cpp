#include "pch.hpp"
#include "Camera.hpp"

#include "Engine/Util/GameSettings.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up,
               float yaw , float pitch ) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), Zoom(ZOOM)
	   {
	       Position = position;
	       WorldUp = up;
	       Yaw = yaw;
	       Pitch = pitch;
	       camera_follow_target_ = nullptr;
	       skybox_tranform = nullptr;

	       follow_offset = { 0, 0.5, 5 };
	       //follow_offset = {0, 2, 10};
	       updateCameraVectors();
	       projection_matrix = glm::perspective(glm::radians(Zoom), static_cast<float>(settings::window_width) / static_cast<float>(settings::window_height), 0.1f, 200.0f);
	       view_matrix = glm::lookAt(Position, Position + Front, Up);
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
