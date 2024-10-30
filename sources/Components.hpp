#pragma once
#include "glm/glm.hpp"
#include <string>


// Component for player-specific data
struct c_player
{
};

// Component for rendering-related data
struct c_render
{
    int layer = 0;
    glm::vec3 color;
};

// Component for position, rotation, and scale
struct c_transform
{
    glm::vec3 position = {0, 0, 0};
    glm::vec3 rotation = {0, 0, 0};
    glm::vec3 scale = {1, 1, 1};
};


// Component for collider-related data
struct c_collider
{
    bool is_trigger = false;
    unsigned int collision_bitmask = 0xFFFFFFFF; // Default bitmask allowing all collisions

};

struct c_health
{
    float health = 100;
};

struct c_enemy
{
    unsigned short target;
    float speed = 1.0f;
    glm::vec3 direction = {0,0,0};
    float cooldown = 2.0f;
    float time_since_shoot = 0.0f;
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
struct c_ui{
    bool follow_cursor = false;
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
struct c_point_light
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
struct c_dynamic_body
{
    float drag = 0.0f;
    float elasticity = 0.1f;
    glm::vec3 velocity = {0, 0, 0};
    glm::vec3 acceleration = {0, 0, 0};
    float angluar_drag = 0.0f;
    glm::vec3 angular_velocity = {0,0,0};
    glm::vec3 angular_acceleration = {0,0,0};
    //glm::vec3 gravity = {0,0,0};
    //glm::vec3 gravity = {0, -9.81f, 0};
    //need listener to check if this entities collider collides with something with ground tag and update grounded bool
};
struct c_damage
{
    float damage = 10;
};
struct c_background
{

};
struct c_asteroid
{
    unsigned short target;
    float speed;
};
struct c_repeat_acceleration
{
    glm::vec3 acceleration = {0,0,0};
    glm::vec3 angularAcceleration = {0,0,0};
};


