#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP
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
    Vec2 acceleration = {0, 0};
};

// Component for entities with a sprite
struct CSprite {};

// Marker component for static bodies (e.g., walls)
struct CStaticBody {};

// Component for entities with a position and rotation
struct CPosition {
    float x = 0;
    float y = 0;
};

// Marker component for kinetic bodies (e.g., moving platforms)
struct CKineticBody {};

// Component for entities with velocity
struct CVelocity {
    float x = 0;
    float y = 0;
};

#endif // COMPONENTS_HPP