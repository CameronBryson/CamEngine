#include "SCamera.hpp"

#include "Engine/EngineUtil.hpp"
#include "Math/MathUtil.hpp"
#include <Engine/GameManager.hpp>
#include "Graphics/Camera.hpp"

static constexpr float camera_movespeed = 100.0f;
static constexpr float camera_rotationspeed = 500.0f;

void s_camera::update(Camera & camera, const float dt)
{
    //eventually change to lerp
    glm::vec3 camera_position = camera.camera_follow_target_ ? camera.camera_follow_target_->position + camera.follow_offset : camera.Position;
    const glm::vec3 forward = camera.Front;
    const glm::vec3 right = camera.Right;
    const glm::vec3 up = camera.Up;

    bool I = engine_util::is_key_pressed('I');
    bool K = engine_util::is_key_pressed('K');
    bool J = engine_util::is_key_pressed('J');
    bool L = engine_util::is_key_pressed('L');
    bool U = engine_util::is_key_pressed('U');
    bool O = engine_util::is_key_pressed('O');
    bool UP = engine_util::is_key_pressed(GLFW_KEY_UP);
    bool DOWN = engine_util::is_key_pressed(GLFW_KEY_DOWN);
    bool LEFT = engine_util::is_key_pressed(GLFW_KEY_LEFT);
    bool RIGHT = engine_util::is_key_pressed(GLFW_KEY_RIGHT);

    if (camera.camera_follow_target_ == nullptr) {
        if (I) camera_position += forward * camera_movespeed * dt;
        if (K) camera_position -= forward * camera_movespeed * dt;
        if (J) camera_position -= right * camera_movespeed * dt;
        if (L) camera_position += right * camera_movespeed * dt;
        if (U) camera_position += up * camera_movespeed * dt;
        if (O) camera_position -= up * camera_movespeed * dt;
    } else {
        if (I) camera.follow_offset += forward * camera_movespeed * dt;
        if (K) camera.follow_offset -= forward * camera_movespeed * dt;
        if (J) camera.follow_offset -= right * camera_movespeed * dt;
        if (L) camera.follow_offset += right * camera_movespeed * dt;
        if (U) camera.follow_offset += up * camera_movespeed * dt;
        if (O) camera.follow_offset -= up * camera_movespeed * dt;
    }

    if (UP) camera.Pitch += camera_rotationspeed * dt;
    if (DOWN) camera.Pitch -= camera_rotationspeed * dt;
    if (LEFT) camera.Yaw -= camera_rotationspeed * dt;
    if (RIGHT) camera.Yaw += camera_rotationspeed * dt;

//    double current_mouse_x, current_mouse_y;
//    glfwGetCursorPos(game_manager::get_glfw_window(), &current_mouse_x, &current_mouse_y);
//    double delta_x = current_mouse_x - camera.previous_mouse_x;
//    double delta_y = current_mouse_y - camera.previous_mouse_y;
//    camera.previous_mouse_x = current_mouse_x;
//    camera.previous_mouse_y = current_mouse_y;
//
//    camera.Yaw += delta_x * camera_rotationspeed * dt * 0.1;
//    camera.Pitch += delta_y * camera_rotationspeed * dt * 0.1;

    camera.Position = camera_position;
    camera.updateCameraVectors();
}