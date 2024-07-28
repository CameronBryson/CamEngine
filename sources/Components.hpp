#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP
#include "Quaternion.hpp"
#include "Vectors.hpp"


// Component for entities affected by gravity
struct CGravity {
    Vec3 gravity = {0, -9.81f, 0};
};

// Component for player-specific data
struct CPlayer {
};

// Component for rendering-related data
struct CRender {
    int layer = 0;
};

// Component for physical properties
struct CRigidBody {
    float mass = 1.0f; // Default mass to avoid division by zero
    float drag = 0.0f;
    Vec3 acceleration = {0, 0, 0};
    Vec3 force = {0, 0, 0};
};

// Component for sprite-related data
struct CSprite {
};

// Marker component for static bodies (e.g., walls)
struct CStaticBody {};

// Component for position, rotation, and scale
struct CTransform {
    Vec3 position = {0, 0, 0};
    Quat rotation = {1, 0, 0, 0};
    Vec3 scale = {1, 1, 1};
};

// Marker component for kinetic bodies (e.g., moving platforms)
struct CKineticBody {};

// Component for velocity
struct CVelocity {
    Vec3 velocity = {0, 0, 0};
};

// Component for collider-related data
struct CCollider {
};

struct CHealth {
    int health = 100;
};

struct CEnemy {
};

struct CAABB {
    Vec3 extents;
};
struct CSphere {
    float radius;
};
struct CCapsule {
    float radius;
    float height;
};
struct CPlane {
    Vec3 normal;
    float distance;
};

#endif // COMPONENTS_HPP