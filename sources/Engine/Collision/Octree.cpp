#include "Octree.hpp"

Octree::Octree(const BoundingBox& boundary, int capacity)
    : root(std::make_unique<OctreeNode>(boundary, capacity))
{

}

bool Octree::insert(const OctreeObject& point) const
{
    return root->insert(point);
}

bool Octree::remove(const OctreeObject& point) const
{
    return root->remove(point);
}

bool Octree::remove(unsigned short entityID) const
{
    return root->remove(entityID);
}

std::vector<OctreeObject> Octree::queryRange(const BoundingBox& range) const
{
    std::vector<OctreeObject> foundPoints;
    root->queryRange(range, foundPoints);
    return foundPoints;
}

void Octree::getPotentialCollisions(std::vector<std::pair<unsigned short, unsigned short>>& collisionPairs) const {
    if (root)
    {
		root->collectPotentialCollisions(collisionPairs);
    }
}

