#pragma once

#include <glm/glm.hpp>
#include <array>

const unsigned int MAX_POINT_LIGHTS = 1;
const unsigned int MAX_SPOT_LIGHTS = 1;
const unsigned int MAX_DIRECTIONAL_LIGHTS = 1;

const unsigned int CAMERA_BINDING = 0;
const unsigned int LIGHT_BINDING = 1;


struct CameraData
{
	glm::vec4 cameraPos;   // Camera position (xyz used, w is padding)
	glm::mat4 view;        // View matrix
	glm::mat4 projection;  // Projection matrix
	glm::mat4 viewProjection;
	glm::mat4 inverseView;
	glm::mat4 inverseProjection;
	glm::mat4 inverseViewProjection;
	glm::mat4 previousView;
	glm::mat4 previousProjection;
	glm::mat4 previousViewProjection;
};


struct PointLightData
{
	glm::vec4 position;    // (x,y,z = position, w = unused)
	glm::vec4 ambient;     // (x,y,z = ambient, w = unused)
	glm::vec4 diffuse;     // (x,y,z = diffuse, w = unused)
	glm::vec4 specular;    // (x,y,z = specular, w = unused)
	// Pack attenuation factors into one vec4:
	// x = constant, y = linear, z = quadratic, w = padding.
	glm::vec4 attenuation;
	std::array<glm::mat4, 6> shadowMatrices;
};


struct SpotLightData
{
	glm::vec4 position;    // (xyz = position)
	glm::vec4 direction;   // (xyz = direction)
	glm::vec4 ambient;     // (rgb ambient)
	glm::vec4 diffuse;     // (rgb diffuse)
	glm::vec4 specular;    // (rgb specular)
	// Attenuation factors: (constant, linear, quadratic, pad)
	glm::vec4 attenuation;
	// Cutoffs: x = inner cutoff, y = outer cutoff, z/w = padding.
	glm::vec4 cutoffs;
	glm::mat4 lightSpaceMatrix;
};

struct DirectionalLightData
{
	glm::vec4 direction;   // (xyz = light direction, w = unused)
	glm::vec4 ambient;     // (rgb ambient, w unused)
	glm::vec4 diffuse;     // (rgb diffuse, w unused)
	glm::vec4 specular;    // (rgb specular, w unused)
	glm::mat4 lightSpaceMatrix;
};

struct LightData
{
	// We need to store the number of lights of each type.
	// To guarantee std140 alignment (each vec4 occupies 16 bytes) we pack
	// the three counts into a single ivec4.
	// counts.x = number of point lights
	// counts.y = number of spot lights
	// counts.z = number of directional lights
	// counts.w = padding
	glm::ivec4 counts;

	// Arrays of light data.
	PointLightData       pointLights[MAX_POINT_LIGHTS];
	SpotLightData        spotLights[MAX_SPOT_LIGHTS];
	DirectionalLightData directionalLights[MAX_DIRECTIONAL_LIGHTS];
};