#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP
#include "Vectors.hpp"

// Component for entities affected by gravity
struct c_gravity
{
    vec3 gravity = {0, -9.81f, 0};
};

// Component for player-specific data
struct c_player
{
};

// Component for rendering-related data
struct c_render
{
    int layer = 0;
};

// Component for physical properties
struct c_rigid_body
{
    float mass = 1.0f; // Default mass to avoid division by zero
    float drag = 0.0f;
    vec3 acceleration = {0, 0, 0};
    vec3 force = {0, 0, 0};
};

// Component for sprite-related data
struct c_sprite
{
};

// Marker component for static bodies (e.g., walls)
struct c_static_body
{
};

// Component for position, rotation, and scale
struct c_transform
{
    vec3 position = {0, 0, 0};
    vec3 rotation = {0, 0, 0};
    vec3 scale = {1, 1, 1};
};

// Marker component for kinetic bodies (e.g., moving platforms)
struct c_kinetic_body
{
};

// Component for velocity
struct c_velocity
{
    vec3 velocity = {0, 0, 0};
};

// Component for collider-related data
struct c_collider
{
};

struct c_health
{
    int health = 100;
};

struct c_enemy
{
};

struct c_aabb
{
    vec3 extents;
};
struct c_sphere
{
    float radius;
};
struct c_capsule
{
    float radius;
    float height;
};
struct c_plane
{
    vec3 normal;
    float distance{};
};

#endif // COMPONENTS_HPP