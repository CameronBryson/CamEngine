#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP
#include "../cmake-build-debug-clang/_deps/raylib-src/src/raylib.h"
#include "Quaternion.hpp"
#include "Vectors.hpp"
// Component for entities affected by gravity
struct CGravity {};

// Marker component for player entities
struct CPlayer {};

// Component for entities that need rendering
struct CRender {
    Vec2 renderOffset = {0, 0};
};

// Component for entities with physical properties
struct CRigidBody {
    float mass = 1.0f; // Default mass to avoid division by zero
    float drag = 0.0f;
    Vec3 acceleration = {0, 0, 0};
    Vec3 force = {0, 0, 0};
};

// Component for entities with a sprite
struct CSprite {};

// Marker component for static bodies (e.g., walls)
struct CStaticBody {};

// Component for entities with a position and rotation
struct CTransform {
    Vec3 position = {0, 0,0};
    Quat rotation = {1, 0, 0, 0};
    Vec3 scale = {1, 1, 1};
};

// Marker component for kinetic bodies (e.g., moving platforms)
struct CKineticBody {};

// Component for entities with velocity
struct CVelocity {
    Vec3 velocity = {0, 0, 0};
};
struct CCollider {};

#endif // COMPONENTS_HPP