#include "SCamera.hpp"

#include "Engine/EngineUtil.hpp"
#include "Math/MathUtil.hpp"
#include <Engine/GameManager.hpp>
#include <raylib.h>

static constexpr float camera_movespeed = 5.0f;
static constexpr float camera_rotationspeed = 1.0f;

void s_camera::update(camera &camera, const float dt)
{
    const vec3 forward = (camera.target - camera.position).normalized();
    const vec3 right = forward.cross_product(camera.up).normalized();
    const vec3 up = camera.up;
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
    if (I)
    {
        camera.position += forward * camera_movespeed * dt;
        moved = true;
    }
    if (K)
    {
        camera.position -= forward * camera_movespeed * dt;
        moved = true;
    }
    if (J)
    {
        camera.position += right * camera_movespeed * dt;
        moved = true;
    }
    if (L)
    {
        camera.position -= right * camera_movespeed * dt;
        moved = true;
    }
    if (U)
    {
        camera.position += up * camera_movespeed * dt;
        moved = true;
    }
    if (O)
    {
        camera.position -= up * camera_movespeed * dt;
        moved = true;
    }

    camera.target = camera.position + forward;

    // Rotate camera
    if (UP)
    {
        camera.target.y += camera_rotationspeed * dt;
        moved = true;
    }
    if (DOWN)
    {
        camera.target.y -= camera_rotationspeed * dt;
        moved = true;
    }
    if (LEFT)
    {
        camera.target.x -= camera_rotationspeed * dt;
        moved = true;
    }
    if (RIGHT)
    {
        camera.target.x += camera_rotationspeed * dt;
        moved = true;
    }
    if(moved)
    {
        camera.update_view_matrix();
    }
}
