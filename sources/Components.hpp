#pragma once
#include "Graphics/Color.hpp"
#include "glm/glm.hpp"
#include <string>

enum class object_collision_type
{
    STATIC,
    KINEMATIC,
    DYNAMIC
};

// Component for entities affected by gravity
struct c_gravity
{
    glm::vec3 gravity = {0, -9.81f, 0};
};

// Component for player-specific data
struct c_player
{
};

// Component for rendering-related data
struct c_render
{
    int layer = 0;
    color color_color;
};

// Component for physical properties
struct c_rigid_body
{
    float mass = 1.0f; // Default mass to avoid division by zero
    float drag = 0.0f;
    glm::vec3 acceleration = {0, 0, 0};
};

// Component for position, rotation, and scale
struct c_transform
{
    glm::vec3 position = {0, 0, 0};
    glm::vec3 rotation = {0, 0, 0};
    glm::vec3 scale = {1, 1, 1};
};

// Component for velocity
struct c_velocity
{
    glm::vec3 velocity = {0, 0, 0};
};

// Component for collider-related data
struct c_collider
{
    bool is_trigger = false;
    unsigned int collision_bitmask = 0xFFFFFFFF; // Default bitmask allowing all collisions
    object_collision_type collision_type = object_collision_type::STATIC;
};

struct c_health
{
    int health = 100;
};

struct c_enemy
{
};

struct c_quad
{
    glm::vec3 extents;
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
    glm::vec3 normal;
    float distance;
};

struct c_model
{
    std::string name;
};
struct c_directional_light
{
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};
