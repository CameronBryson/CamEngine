#pragma once
#include <memory> 
#include "OctreeNode.hpp"
class Octree
{
public:
    Octree(const BoundingBox& boundary, int capacity);
    ~Octree() = default; 

    bool insert(const OctreeObject& obj) const;
	bool remove(const OctreeObject& obj) const;
	bool remove(unsigned short entityID) const;
    std::vector<OctreeObject> queryRange(const BoundingBox& range) const;
	void getPotentialCollisions(std::vector<std::pair<unsigned short, unsigned short>>& collisionPairs) const;


private:
    std::unique_ptr<OctreeNode> root; 
};
