#include "Engine/pch.hpp"
#include "OctreeNode.hpp"

OctreeNode::OctreeNode(const BoundingBox& bounds, int capacity)
    : bounds(bounds), capacity(capacity), divided(false)
{
}

bool OctreeNode::insert(const OctreeObject& obj)
{
    // If the object's bounds do not intersect this node's bounds, return false
    if (!bounds.intersects(obj.bounds))
    {
        return false;
    }

    // If there's capacity and the node is not divided, add the object here
    if (objects.size() < capacity && !divided)
    {
        objects.push_back(obj);
        return true;
    }

    // Subdivide if necessary
    if (!divided)
    {
        subdivide();
    }

    // Try to insert the object into child nodes
    bool insertedIntoChild = false;
    for (auto& child : children)
    {
        if (child->insert(obj))
        {
            insertedIntoChild = true;
        }
    }

    // If the object doesn't fit into any child, keep it in this node
    if (!insertedIntoChild)
    {
        objects.push_back(obj);
    }

    return true;
}

bool OctreeNode::remove(const OctreeObject& obj)
{
    // If the object's bounds do not intersect this node's bounds, it cannot be here
    if (!bounds.intersects(obj.bounds))
    {
        return false;
    }

    // Attempt to find and remove the object from this node
    auto it = std::find_if(objects.begin(), objects.end(),
        [&](const OctreeObject& o) { return o.entityID == obj.entityID; });

    if (it != objects.end())
    {
        objects.erase(it);
        return true;
    }

    // Recursively attempt to remove the object from child nodes
    if (divided)
    {
        for (auto& child : children)
        {
            if (child && child->remove(obj))
            {
                // Optionally consolidate child nodes here
                consolidate();
                return true;
            }
        }
    }

    // Object not found in this node or its descendants
    return false;
}

bool OctreeNode::remove(unsigned short entityID)
{
    // Attempt to find and remove the object from this node
    auto it = std::find_if(objects.begin(), objects.end(),
        [&](const OctreeObject& o) { return o.entityID == entityID; });

    if (it != objects.end())
    {
        objects.erase(it);
        return true;
    }

    // If the node is divided, attempt to remove the object from child nodes
    if (divided)
    {
        for (auto& child : children)
        {
            if (child && child->remove(entityID))
            {
                // After removal, check if children can be consolidated
                consolidate();
                return true;
            }
        }
    }

    // Object not found in this node or its descendants
    return false;
}

void OctreeNode::subdivide()
{
	const glm::vec3& min = bounds.min;
	const glm::vec3& max = bounds.max;
	glm::vec3 center = (min + max) * 0.5f;

	// Create bounding boxes for each child
	std::array<BoundingBox, 8> octants = {
	    // Octant definitions...
	};

	// Create child nodes using unique_ptr
	for (int i = 0; i < 8; ++i)
	{
		children[i] = std::make_unique<OctreeNode>(octants[i], capacity);
	}

	divided = true;

	// Temporary vector to hold objects that remain in this node
	std::vector<OctreeObject> remainingObjects;

	// Move existing objects into appropriate child nodes
	for (const auto& obj : objects)
	{
		bool insertedIntoChild = false;
		for (auto& child : children)
		{
			if (child->insert(obj))
			{
				insertedIntoChild = true;
				// Do not break; allow insertion into all relevant child nodes
			}
		}

		// If the object doesn't fit into any child, keep it in this node
		if (!insertedIntoChild)
		{
			remainingObjects.push_back(obj);
		}
	}

	// Replace the objects in the current node with remaining ones
	objects = std::move(remainingObjects);
}

void OctreeNode::consolidate()
{
    // Count total objects in all children
    int totalObjects = static_cast<int>(objects.size());
    for (const auto& child : children)
    {
        if (child)
        {
            totalObjects += static_cast<int>(child->objects.size());
        }
    }

    // If total objects are less than capacity, merge them
    if (totalObjects <= capacity)
    {
        // Collect objects from children
        for (auto& child : children)
        {
            if (child)
            {
                objects.insert(objects.end(), child->objects.begin(), child->objects.end());
                child.reset(); // Delete the child node
            }
        }
        divided = false;
    }
}

void OctreeNode::queryRange(const BoundingBox& range, std::vector<OctreeObject>& foundObjects) const
{
    // If the range doesn't intersect this node's bounds, return
    if (!bounds.intersects(range))
    {
        return;
    }

    // Check objects at this node
    for (const auto& obj : objects)
    {
        if (range.intersects(obj.bounds))
        {
            foundObjects.push_back(obj);
        }
    }

    // If the node is divided, recursively check children
    if (divided)
    {
        for (const auto& child : children)
        {
            child->queryRange(range, foundObjects);
        }
    }
}

void OctreeNode::collectPotentialCollisions(
    std::vector<std::pair<unsigned short, unsigned short>>& collisionPairs) const
{
	// Collect potential collisions between objects within this node
	if (objects.size() > 1)
	{
		for (size_t i = 0; i < objects.size(); ++i)
		{
			for (size_t j = i + 1; j < objects.size(); ++j)
			{
				collisionPairs.emplace_back(objects[i].entityID, objects[j].entityID);
			}
		}
	}

	// If the node has children
	if (divided)
	{
		// Collect collisions between objects in this node and in child nodes
		for (const auto& child : children)
		{
			if (child)
			{
				// Collide objects in current node with objects in child node
				child->collectCollisionsWithParentObjects(objects, collisionPairs);

				// Recurse into child node
				child->collectPotentialCollisions(collisionPairs);
			}
		}
	}
}

void OctreeNode::collectCollisionsWithParentObjects(
    const std::vector<OctreeObject>& parentObjects,
    std::vector<std::pair<unsigned short, unsigned short>>& collisionPairs) const
{
	// Collide objects in parentObjects with objects in this node
	for (const auto& parentObj : parentObjects)
	{
		for (const auto& obj : objects)
		{
			collisionPairs.emplace_back(parentObj.entityID, obj.entityID);
		}
	}

	// If the node has children, recurse
	if (divided)
	{
		for (const auto& child : children)
		{
			if (child)
			{
				child->collectCollisionsWithParentObjects(parentObjects, collisionPairs);
			}
		}
	}
}
