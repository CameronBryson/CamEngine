#pragma once
#include "glm/glm.hpp"
#include <string>


// Component for player-specific data
struct CPlayer
{
};

// Component for rendering-related data
struct CRender
{
    int layer = 0;
    glm::vec3 color;
};

// Component for position, rotation, and scale
struct CTransform
{
    glm::vec3 position = {0, 0, 0};
    glm::vec3 rotation = {0, 0, 0};
    glm::vec3 scale = {1, 1, 1};
};


// Component for collider-related data
struct CCollider
{
    bool is_trigger = false;
    unsigned int collision_bitmask = 0xFFFFFFFF; // Default bitmask allowing all collisions

};

struct CHealth
{
    float health = 100;
};

struct CEnemy
{
    unsigned short target;
    float speed = 1.0f;
    glm::vec3 direction = {0,0,0};
    float cooldown = 2.0f;
    float time_since_shoot = 0.0f;
};

struct CQuad
{
    glm::vec3 extents;
};

struct CSphere
{
    float radius;
};


struct CUI{
    bool follow_cursor = false;
};

struct CModel
{
    std::string name;
};
struct CDirectionalLight
{
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};
struct CPointLight
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
struct CDynamicBody
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
struct CDamage
{
    float damage = 10;
};
struct CBackground
{

};
struct CAsteroid
{
    unsigned short target;
    float speed;
};
struct CRepeatAcceleration
{
    glm::vec3 acceleration = {0,0,0};
    glm::vec3 angularAcceleration = {0,0,0};
};


