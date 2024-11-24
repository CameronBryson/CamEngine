#pragma once
#include <glm/vec3.hpp>
struct BoundingBox
{
	glm::vec3 min;
	glm::vec3 max;
	BoundingBox()
		: min(0.0f), max(0.0f)
	{
	}
	BoundingBox(const glm::vec3& min, const glm::vec3& max)
		: min(min), max(max)
	{
	}

	// Check if a point is within this bounding box
	bool containsPoint(const glm::vec3& point) const
	{
		return (point.x >= min.x && point.x <= max.x) &&
			(point.y >= min.y && point.y <= max.y) &&
			(point.z >= min.z && point.z <= max.z);
	}

	// Check if this bounding box intersects with another
	bool intersects(const BoundingBox& other) const
	{
		return (min.x <= other.max.x && max.x >= other.min.x) &&
			(min.y <= other.max.y && max.y >= other.min.y) &&
			(min.z <= other.max.z && max.z >= other.min.z);
	}
};