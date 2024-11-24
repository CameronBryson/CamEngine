#pragma once
#include "glm/glm.hpp"

struct Vertex
{
    Vertex() {}
	Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& texture_coordinates) : position(position), normal(normal), texture_coordinates(texture_coordinates) {}
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coordinates;
};
