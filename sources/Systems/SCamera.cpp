#include "SCamera.hpp"

#include "Engine/EngineUtil.hpp"
#include "Math/MathUtil.hpp"
#include <Engine/GameManager.hpp>

static constexpr float camera_movespeed = 100.0f;
static constexpr float camera_rotationspeed = 500.0f;

void s_camera::update(Camera & camera, const float dt)
{
    glm::vec3 camera_position = camera.Position;
    const glm::vec3 forward = camera.Front;
    const glm::vec3 right = camera.Right;
    const glm::vec3 up = camera.Up;
    bool moved = false;
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

    // Move camera
    if( I )
    {
        camera_position += forward * camera_movespeed * dt;
        moved = true;
    }
    if( K )
    {
        camera_position -= forward * camera_movespeed * dt;
        moved = true;
    }
    if( J )
    {
        camera_position -= right * camera_movespeed * dt;
        moved = true;
    }
    if( L )
    {
        camera_position += right * camera_movespeed * dt;
        moved = true;
    }
    if( U )
    {
        camera_position += up * camera_movespeed * dt;
        moved = true;
    }
    if( O )
    {
        camera_position -= up * camera_movespeed * dt;
        moved = true;
    }

    if( UP )
    {
        camera.Pitch += camera_rotationspeed * dt;
        moved = true;
    }
    if( DOWN )
    {
        camera.Pitch -= camera_rotationspeed * dt;
        moved = true;
    }
    if( LEFT )
    {
        camera.Yaw -= camera_rotationspeed * dt;
        moved = true;
    }
    if( RIGHT )
    {
        camera.Yaw += camera_rotationspeed * dt;
        moved = true;
    }
    if( moved )
    {
        camera.Position = camera_position;
        camera.updateCameraVectors();
    }
}