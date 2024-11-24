#pragma once
#include <string>

#include "glm/vec3.hpp"

// Component for rendering-related data
struct CRender
{
    CRender(const int layer, const glm::vec3 color) : layer(layer), color(color) {}
    int layer = 0;
    glm::vec3 color;
};

// Component for position, rotation, and scale
struct CTransform
{
    CTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) : position(position), rotation(rotation), scale(scale) {}
    glm::vec3 position = {0, 0, 0};
    glm::vec3 rotation = {0, 0, 0};
    glm::vec3 scale = {1, 1, 1};
};

// Component for collider-related data
struct CCollider
{
    CCollider(const bool is_trigger, const unsigned int collision_bitmask) : is_trigger(is_trigger), collision_bitmask(collision_bitmask) {}
    bool is_trigger = false;
    unsigned int collision_bitmask = 0xFFFFFFFF; // Default bitmask allowing all collisions
};



struct CBoxBounds
{
    CBoxBounds(const glm::vec3& extents) : extents(extents) {}
    glm::vec3 extents;
};

struct CSphereBounds
{
    CSphereBounds(float radius) : radius(radius) {}
    float radius;
};

struct CUI
{
    bool follow_cursor = false;
};

struct CModel
{
    CModel(const std::string& name) : name(name) {}
    std::string name;
};

struct CDirectionalLight
{
    CDirectionalLight(const glm::vec3& direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
        : direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) {}
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct CPointLight
{
    CPointLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const float constant, const float linear, const float quadratic)
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
    CDynamicBody(const float elasticity, const float drag, const float angularDrag)
        : elasticity(elasticity), drag(drag),  angularDrag(angularDrag) {}
    float elasticity = 0.1f;
    float drag = 0.0f;
    glm::vec3 velocity = {0, 0, 0};
    glm::vec3 acceleration = {0, 0, 0};
    float angularDrag = 0.0f;
    glm::vec3 angularVelocity = {0, 0, 0};
    glm::vec3 angularAcceleration = {0, 0, 0};
};


struct CBackground
{
    CBackground() = default;
};


struct CRepeatAcceleration
{
    CRepeatAcceleration(const glm::vec3& acceleration, const glm::vec3& angularAcceleration) : acceleration(acceleration), angularAcceleration(angularAcceleration) {}
    glm::vec3 acceleration = {0, 0, 0};
    glm::vec3 angularAcceleration = {0, 0, 0};
};
