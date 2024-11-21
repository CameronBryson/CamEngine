#include "Octree.hpp"

Octree::Octree(const BoundingBox& boundary, int capacity)
    : root(std::make_unique<OctreeNode>(boundary, capacity))
{

}

bool Octree::insert(const OctreePoint& point)
{
    return root->insert(point);
}

bool Octree::remove(const OctreePoint& point)
{
    return root->remove(point);
}

bool Octree::remove(unsigned short entityID)
{
    return root->remove(entityID);
}

std::vector<OctreePoint> Octree::queryRange(const BoundingBox& range) const
{
    std::vector<OctreePoint> foundPoints;
    root->queryRange(range, foundPoints);
    return foundPoints;
}

