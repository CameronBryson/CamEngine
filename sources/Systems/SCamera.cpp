#include "SCamera.hpp"
#include "Math/MathUtil.hpp"
#include <raylib.h>

static constexpr float camera_movespeed = 5.0f;
static constexpr float camera_rotationspeed = 1.0f;

void s_camera::update(camera &camera, const float dt)
{
    const vec3 forward = (camera.target - camera.position).normalized();
    const vec3 right = forward.cross_product(camera.up).normalized();
    const vec3 up = camera.up;
    bool moved = false;
    bool I;
    bool K;
    bool J;
    bool L;
    bool U;
    bool O;
    bool UP;
    bool DOWN;
    bool LEFT;
    bool RIGHT;

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
