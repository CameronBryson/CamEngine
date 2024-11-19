#pragma once
#include "glm/glm.hpp"
#include <string>

// Component for player-specific data
struct CPlayer
{
    CPlayer() = default;
};

// Component for rendering-related data
struct CRender
{
    CRender(int layer, glm::vec3 color) : layer(layer), color(color) {}
    int layer = 0;
    glm::vec3 color;
};

// Component for position, rotation, and scale
struct CTransform
{
    CTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) : position(position), rotation(rotation), scale(scale) {}
    glm::vec3 position = {0, 0, 0};
    glm::vec3 rotation = {0, 0, 0};
    glm::vec3 scale = {1, 1, 1};
};

// Component for collider-related data
struct CCollider
{
    CCollider(bool is_trigger, unsigned int collision_bitmask) : is_trigger(is_trigger), collision_bitmask(collision_bitmask) {}
    bool is_trigger = false;
    unsigned int collision_bitmask = 0xFFFFFFFF; // Default bitmask allowing all collisions
};

struct CHealth
{
    CHealth(float health) : health(health) {}
    float health = 100;
};

struct CEnemy
{
    CEnemy(unsigned short target, float speed, glm::vec3 direction) : target(target), speed(speed), direction(direction) {}
    unsigned short target;
    float speed = 1.0f;
    glm::vec3 direction = {0, 0, 0};
    float cooldown = 2.0f;
    float time_since_shoot = 0.0f;
};

struct CQuad
{
    CQuad(glm::vec3 extents) : extents(extents) {}
    glm::vec3 extents;
};

struct CSphere
{
    CSphere(float radius) : radius(radius) {}
    float radius;
};

struct CUI
{
    bool follow_cursor = false;
};

struct CModel
{
    CModel(std::string name) : name(name) {}
    std::string name;
};

struct CDirectionalLight
{
    CDirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
        : direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) {}
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct CPointLight
{
    CPointLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic)
        : ambient(ambient), diffuse(diffuse), specular(specular), constant(constant), linear(linear), quadratic(quadratic) {}
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct CDynamicBody
{
    CDynamicBody(float elasticity, float drag, float angluar_drag)
        : elasticity(elasticity), drag(drag),  angluar_drag(angluar_drag) {}
    float elasticity = 0.1f;
    float drag = 0.0f;
    glm::vec3 velocity = {0, 0, 0};
    glm::vec3 acceleration = {0, 0, 0};
    float angluar_drag = 0.0f;
    glm::vec3 angular_velocity = {0, 0, 0};
    glm::vec3 angular_acceleration = {0, 0, 0};
};

struct CDamage
{
    CDamage(float damage) : damage(damage) {}
    float damage = 10;
};

struct CBackground
{
    CBackground() = default;
};

struct CAsteroid
{
    CAsteroid(unsigned short target, float speed) : target(target), speed(speed) {}
    unsigned short target;
    float speed;
};

struct CRepeatAcceleration
{
    CRepeatAcceleration(glm::vec3 acceleration, glm::vec3 angularAcceleration) : acceleration(acceleration), angularAcceleration(angularAcceleration) {}
    glm::vec3 acceleration = {0, 0, 0};
    glm::vec3 angularAcceleration = {0, 0, 0};
};
