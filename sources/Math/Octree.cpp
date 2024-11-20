#include "Octree.hpp"

Octree::Octree(const BoundingBox& boundary, int capacity)
    : root(std::make_unique<OctreeNode>(boundary, capacity))
{
}

bool Octree::insert(const OctreePoint& point)
{
    return root->insert(point);
}

std::vector<OctreePoint> Octree::queryRange(const BoundingBox& range) const
{
    std::vector<OctreePoint> foundPoints;
    root->queryRange(range, foundPoints);
    return foundPoints;
}

