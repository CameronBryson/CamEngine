#pragma once
#include "OctreeNode.hpp"
#include <memory> 

class Octree
{
public:
    Octree(const BoundingBox& boundary, int capacity);
    ~Octree() = default; 

    bool insert(const OctreePoint& point) const;
	bool remove(const OctreePoint& point) const;
	bool remove(unsigned short entityID) const;
    std::vector<OctreePoint> queryRange(const BoundingBox& range) const;

private:
    std::unique_ptr<OctreeNode> root; 
};
