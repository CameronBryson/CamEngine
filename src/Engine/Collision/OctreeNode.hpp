#pragma once
#include "BoundngBox.hpp"
#include <vector>
#include <array>
#include <glm/vec3.hpp>
#include <memory>
struct OctreeObject
{
	BoundingBox bounds;
	unsigned short entityID;

	OctreeObject(const BoundingBox& bounds, unsigned short entityID)
		: bounds(bounds), entityID(entityID)
	{

	}
};

class OctreeNode
{
public:
	OctreeNode(const BoundingBox& bounds, int capacity);
	~OctreeNode() = default;

	bool insert(const OctreeObject& obj);
	bool remove(const OctreeObject& obj);
	bool remove(unsigned short entityID);
	void subdivide();
	void consolidate();
	void queryRange(const BoundingBox& range, std::vector<OctreeObject>& foundObjects) const;
	void collectPotentialCollisions(std::vector<std::pair<unsigned short, unsigned short>>& collisionPairs) const;
	void collectCollisionsWithParentObjects(
	    const std::vector<OctreeObject>& parentObjects,
	    std::vector<std::pair<unsigned short, unsigned short>>& collisionPairs) const;


private:
	BoundingBox bounds;
	int capacity;
	std::vector<OctreeObject> objects;
	std::array<std::unique_ptr<OctreeNode>, 8> children;
	bool divided;

};