#include "OctreeNode.hpp"

OctreeNode::OctreeNode(const BoundingBox& bounds, int capacity) : bounds(bounds), capacity(capacity), divided(false)
{
}

bool OctreeNode::insert(const OctreePoint& point)
{
	// Check if the point is within this node's bounds
	if (!bounds.containsPoint(*point.position))
	{
		return false;
	}

	// If there's capacity and the node is not divided, add the point here
	if (points.size() < capacity && !divided)
	{
		points.push_back(point);
		return true;
	}

	// Otherwise, subdivide if not already divided
	if (!divided)
	{
		subdivide();
	}

	// Try to insert the point into a child node
	for (auto& child : children)
	{
		if (child->insert(point))
		{
			return true;
		}
	}

	// Should not reach here
	return false;
}
bool OctreeNode::remove(const OctreePoint& point)
{
	// If the point is not within this node's bounds, it cannot be here
	if (!bounds.containsPoint(*point.position))
	{
		return false;
	}

	// Attempt to find and remove the point from this node
	auto it = std::find_if(points.begin(), points.end(),
		[&](const OctreePoint& p) { return p.entityID == point.entityID; });

	if (it != points.end())
	{
		points.erase(it);
		return true;
	}

	// Recursively attempt to remove the point from child nodes
	if (divided)
	{
		for (auto& child : children)
		{
			if (child && child->remove(point))
			{
				// Optionally consolidate child nodes here
				consolidate();
				return true;
			}
		}
	}

	// Point not found in this node or its descendants
	return false;
}

bool OctreeNode::remove(unsigned short entityID)
{
	// Attempt to find and remove the point from this node
	auto it = std::find_if(points.begin(), points.end(),
		[&](const OctreePoint& p) { return p.entityID == entityID; });

	if (it != points.end())
	{
		points.erase(it);
		return true;
	}

	// If the node is divided, attempt to remove the point from child nodes
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

	// Point not found in this node or its descendants
	return false;
}

void OctreeNode::subdivide() {
	const glm::vec3& min = bounds.min;
	const glm::vec3& max = bounds.max;
	glm::vec3 center = (min + max) * 0.5f;

	// Create bounding boxes for each child
	std::array<BoundingBox, 8> octants = {
		// Octant 0: left-bottom-back
		BoundingBox(min, center),

		// Octant 1: right-bottom-back
		BoundingBox({center.x, min.y, min.z}, {max.x, center.y, center.z}),

		// Octant 2: left-top-back
		BoundingBox({min.x, center.y, min.z}, {center.x, max.y, center.z}),

		// Octant 3: right-top-back
		BoundingBox({center.x, center.y, min.z}, {max.x, max.y, center.z}),

		// Octant 4: left-bottom-front
		BoundingBox({min.x, min.y, center.z}, {center.x, center.y, max.z}),

		// Octant 5: right-bottom-front
		BoundingBox({center.x, min.y, center.z}, {max.x, center.y, max.z}),

		// Octant 6: left-top-front
		BoundingBox({min.x, center.y, center.z}, {center.x, max.y, max.z}),

		// Octant 7: right-top-front
		BoundingBox(center, max)
	};

	// Create child nodes using unique_ptr
	for (int i = 0; i < 8; ++i) {
		children[i] = std::make_unique<OctreeNode>(octants[i], capacity);
	}

	divided = true;

	// Move existing points into appropriate child nodes
	for (const auto& point : points) {
		for (auto& child : children) {
			if (child->insert(point)) {
				break;
			}
		}
	}

	// Clear the points from the current node
	points.clear();
}

void OctreeNode::consolidate()
{
	// Count total points in all children
	int totalPoints = static_cast<int>(points.size());
	for (const auto& child : children)
	{
		if (child)
		{
			totalPoints += static_cast<int>(child->points.size());
		}
	}

	// If total points are less than capacity, merge them
	if (totalPoints <= capacity)
	{
		// Collect points from children
		for (auto& child : children)
		{
			if (child)
			{
				points.insert(points.end(), child->points.begin(), child->points.end());
				child.reset(); // Delete the child node
			}
		}
		divided = false;
	}
}


void OctreeNode::queryRange(const BoundingBox& range, std::vector<OctreePoint>& foundPoints) const {
	// If the range doesn't intersect this node's bounds, return
	if (!bounds.intersects(range)) {
		return;
	}

	// Check points at this node
	for (const auto& point : points) {
		if (range.containsPoint(*point.position)) {
			foundPoints.push_back(point);
		}
	}

	// If the node is divided, recursively check children
	if (divided) {
		for (const auto& child : children) {
			child->queryRange(range, foundPoints);
		}
	}
}
