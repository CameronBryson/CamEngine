#include "SCamera.hpp"
#include "MathUtil.hpp"
#include <raylib.h>
#include <string> // Add this include for std::to_string

static constexpr float camera_movespeed = 5.0f;
static constexpr float camera_rotationspeed = 1.0f;

void s_camera::update(camera &camera, const float dt)
{
    const vec3 forward = (camera.target - camera.position).normalized();
    const vec3 right = MathUtil::cross_product(forward, camera.up).normalized();
    const vec3 up = camera.up;

    // Move camera
    if (IsKeyDown(KEY_I)) {
        camera.position += forward * camera_movespeed * dt;
    }
    if (IsKeyDown(KEY_K)) {
        camera.position -= forward * camera_movespeed * dt;
    }
    if (IsKeyDown(KEY_J)) {
        camera.position += right * camera_movespeed * dt;
    }
    if (IsKeyDown(KEY_L)) {
        camera.position -= right * camera_movespeed * dt;
    }
    if (IsKeyDown(KEY_U)) {
        camera.position += up * camera_movespeed * dt;
    }
    if (IsKeyDown(KEY_O)) {
        camera.position -= up * camera_movespeed * dt;
    }

    camera.target = camera.position + forward;

    // Rotate camera
    if (IsKeyDown(KEY_UP)) {
        camera.target.y += camera_rotationspeed * dt;
    }
    if (IsKeyDown(KEY_DOWN)) {
        camera.target.y -= camera_rotationspeed * dt;
    }
    if (IsKeyDown(KEY_LEFT)) {
        camera.target.x -= camera_rotationspeed * dt;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        camera.target.x += camera_rotationspeed * dt;
    }

}