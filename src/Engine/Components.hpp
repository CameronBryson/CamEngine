#pragma once
#include <string>

#include <entt/entity/fwd.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/quaternion.hpp> // Include the complete quaternion header
#include <glm/fwd.hpp>
#include <vector>

// Component for rendering-related data
struct CRender
{
    CRender(const int layer, const glm::vec3 color) : layer(layer), color(color) {}
    int layer = 0;
    glm::vec3 color;
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
    CPointLight(const glm::vec3 position,const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const float constant, const float linear, const float quadratic)
        : position(position), ambient(ambient), diffuse(diffuse), specular(specular), constant(constant), linear(linear), quadratic(quadratic) {}
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
};
struct CSpotLight
{
	CSpotLight(const glm::vec3 position, const glm::vec3 direction, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const float constant, const float linear, const float quadratic, const float innerCutoff, const float outerCutoff)
		: position(position), direction(direction), ambient(ambient), diffuse(diffuse), specular(specular), constant(constant), linear(linear), quadratic(quadratic), innerCutoff(innerCutoff), outerCutoff(outerCutoff) {
	}
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float constant;
	float linear;
	float quadratic;
	float innerCutoff;
	float outerCutoff;
};

struct CText
{
    CText(const std::string& text, const glm::vec2& position, const int font_size, const glm::vec3& color) : text(text), position(position),font_size(font_size), color(color) {}
    std::string text;
    int font_size;
    glm::vec2 position;
    glm::vec3 color;
};

struct CTransform 
{
    CTransform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) : position(position), rotation(rotation), scale(scale), model_matrix(1.0f), dirty(true) {}
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    glm::mat4 model_matrix;
    bool dirty;
};
struct CParent
{
	CParent(entt::entity parent) : parent(parent) {}
	entt::entity parent;
};
struct CChildren
{
	CChildren() : children(children) {}
	std::vector<entt::entity> children;
};