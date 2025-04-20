#pragma once
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

struct Vertex
{
	Vertex() = default;
	Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex)
		: position(pos), normal(norm), texture_coordinates(tex)
	{
	}

	glm::vec3 position = { 0, 0, 0 };
	glm::vec3 normal = { 0, 0, 0 };
	glm::vec2 texture_coordinates = { 0, 0 };
	glm::vec3 tangent = { 0, 0, 0 };
	glm::vec3 bitangent = { 0, 0, 0 };

	bool operator==(const Vertex& other) const
	{
		return position == other.position &&
			normal == other.normal &&
			texture_coordinates == other.texture_coordinates &&
			tangent == other.tangent &&
			bitangent == other.bitangent;
	}
};