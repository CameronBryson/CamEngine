#pragma once
#include "BoundngBox.hpp"
#include <vector>
#include <array>
#include <glm/vec3.hpp>
#include <memory>
#include "Components.hpp"
struct OctreePoint
{
	glm::vec3* position;
	unsigned short entityID;

	OctreePoint(glm::vec3* position, unsigned short entityID)
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
	bool remove(const OctreePoint& point);
	bool remove(unsigned short entityID);
	void subdivide();
	void consolidate();
	void queryRange(const BoundingBox& range, std::vector<OctreePoint>& pointsInRange) const;

private:
	BoundingBox bounds;
	int capacity;
	std::vector<OctreePoint> points;
	std::array<std::unique_ptr<OctreeNode>, 8> children;
	bool divided;

};