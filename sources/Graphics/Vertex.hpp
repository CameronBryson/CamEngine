#pragma once
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
struct Vertex
{
    Vertex() = default;
	Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& textureCoordinates) : position(position), normal(normal), texture_coordinates(textureCoordinates) {}
    glm::vec3 position = { 0,0,0 };
    glm::vec3 normal = {0,0,0};
    glm::vec2 texture_coordinates = {0,0};
};
