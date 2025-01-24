#include "pch.hpp"
#include "CameraController.hpp"
#include "Engine/Util/EngineUtil.hpp"
#include "Engine/Base/BaseScene.hpp"
void CameraController::update(float deltaTime)
{
	//eventually change to lerp
	Camera* m_Camera = &GetScene().mMainCamera;
	glm::vec3 camera_position = m_Camera->Position;
	const glm::vec3 forward = m_Camera->Front;
	const glm::vec3 right = m_Camera->Right;
	const glm::vec3 up = m_Camera->Up;

	bool I = engine_util::isKeyPressed('I');
	bool K = engine_util::isKeyPressed('K');
	bool J = engine_util::isKeyPressed('J');
	bool L = engine_util::isKeyPressed('L');
	bool U = engine_util::isKeyPressed('U');
	bool O = engine_util::isKeyPressed('O');
	bool UP = engine_util::isKeyPressed(GLFW_KEY_UP);
	bool DOWN = engine_util::isKeyPressed(GLFW_KEY_DOWN);
	bool LEFT = engine_util::isKeyPressed(GLFW_KEY_LEFT);
	bool RIGHT = engine_util::isKeyPressed(GLFW_KEY_RIGHT);

	if( I )
		camera_position += forward * camera_movespeed * deltaTime;
	if( K )
		camera_position -= forward * camera_movespeed * deltaTime;
	if( J )
		camera_position -= right * camera_movespeed * deltaTime;
	if( L )
		camera_position += right * camera_movespeed * deltaTime;
	if( U )
		camera_position += up * camera_movespeed * deltaTime;
	if( O )
		camera_position -= up * camera_movespeed * deltaTime;

	if( UP )
	m_Camera->Pitch += camera_rotationspeed * deltaTime;
	if( DOWN )
	m_Camera->Pitch -= camera_rotationspeed * deltaTime;
	if( LEFT )
	m_Camera->Yaw -= camera_rotationspeed * deltaTime;
	if( RIGHT )
	m_Camera->Yaw += camera_rotationspeed * deltaTime;

	//    double current_mouse_x, current_mouse_y;
	//    glfwGetCursorPos(game_manager::get_glfw_window(), &current_mouse_x, &current_mouse_y);
	//    double delta_x = current_mouse_x - camera.previous_mouse_x;
	//    double delta_y = current_mouse_y - camera.previous_mouse_y;
	//    camera.previous_mouse_x = current_mouse_x;
	//    camera.previous_mouse_y = current_mouse_y;
	//
	//    camera.Yaw += delta_x * camera_rotationspeed * dt * 0.1;
	//    camera.Pitch += delta_y * camera_rotationspeed * dt * 0.1;

	//camera.Position = camera_position;
	m_Camera->updateCameraVectors();
}
