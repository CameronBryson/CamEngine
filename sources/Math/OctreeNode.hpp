#pragma once
#include "BoundngBox.hpp"
#include <vector>
#include <array>
#include <glm/vec3.hpp>
#include <memory>
struct OctreePoint
{
	glm::vec3 position;
	unsigned short entityID;

	OctreePoint(const glm::vec3& position, unsigned short entityID)
		: position(position), entityID(entityID)
	{

	}
};

class OctreeNode
{
public:
	OctreeNode(const BoundingBox& bounds, int capacity);
	~OctreeNode() = default;

	bool insert(const OctreePoint& point);
	void subdivide();
	void queryRange(const BoundingBox& range, std::vector<OctreePoint>& pointsInRange) const;

private:
	BoundingBox bounds;
	int capacity;
	std::vector<OctreePoint> points;
	std::array<std::unique_ptr<OctreeNode>, 8> children;
	bool divided;

};