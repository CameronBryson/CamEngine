#include "SCollision.hpp"

#include "../Engine/Collision.hpp"
#include "Engine/Registry.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "Math/Octree.hpp"

void SCollision::update(Registry & registry)
{
	auto & quads = registry.getSparseSet<CBoxBounds>();
	auto & spheres = registry.getSparseSet<CSphereBounds>();
	auto & transforms = registry.getSparseSet<CTransform>();
	auto & colliders = registry.getSparseSet<CCollider>();
	auto & dynamic_bodies = registry.getSparseSet<CDynamicBody>();

	std::vector<unsigned short> collider_ids = registry.getEntityIDs<CCollider, CTransform>();
	std::vector<unsigned short> obb_ids = registry.getEntityIDs<CBoxBounds, CTransform, CCollider>();
	std::vector<unsigned short> sphere_ids = registry.getEntityIDs<CSphereBounds, CTransform, CCollider>();

	BoundingBox sceneBounds(settings::world_boundry_min, settings::world_boundry_max);

	Octree octree(sceneBounds, 8);

	for (auto id : collider_ids)
	{
		auto& transform = transforms.get_item(id);
		octree.insert({ transform.position, id });
	}

	for (auto id : collider_ids)
	{
		auto& transform = transforms.get_item(id);
		BoundingBox queryBounds;
		if (quads.hasItem(id))
		{
			auto& obb = quads.get_item(id);
			queryBounds.min = transform.position - obb.extents;
			queryBounds.max = transform.position + obb.extents;
		}
		else if (spheres.hasItem(id))
		{
			auto& sphere = spheres.get_item(id);
			queryBounds.min = transform.position - glm::vec3(sphere.radius);
			queryBounds.max = transform.position + glm::vec3(sphere.radius);
		}
		else
		{
			// Default query range if collider type is unknown
			queryBounds.min = transform.position - glm::vec3(10.0f);
			queryBounds.max = transform.position + glm::vec3(10.0f);
		}
		std::vector<OctreePoint> potential_colliders = octree.queryRange(queryBounds);

		for (const auto& point : potential_colliders)
		{
			unsigned short otherID = point.entityID;
			if (id >= otherID)
			{
				continue;
			}
			// Determine collider types and perform appropriate collision checks
			bool is_obb_1 = quads.hasItem(id);
			bool is_obb_2 = quads.hasItem(otherID);
			bool is_sphere_1 = spheres.hasItem(id);
			bool is_sphere_2 = spheres.hasItem(otherID);

			if (is_obb_1 && is_obb_2)
			{
				intersectsObbInObb(id, otherID, quads, transforms, colliders, dynamic_bodies);
			}
			else if (is_obb_1 && is_sphere_2)
			{
				intersectsObbInSphere(id, otherID, quads, spheres, transforms, colliders, dynamic_bodies);
			}
			else if (is_sphere_1 && is_obb_2)
			{
				intersectsObbInSphere(otherID, id, quads, spheres, transforms, colliders, dynamic_bodies);
			}
			else if (is_sphere_1 && is_sphere_2)
			{
				intersectsSphereInSphere(id, otherID, spheres, transforms, colliders, dynamic_bodies);
			}
		}
	}
	//for (int i = 0; i < obb_ids.size(); ++i)
	//{
	//	for (int j = i + 1; j < obb_ids.size(); ++j)
	//	{
	//		intersectsObbInObb(obb_ids[i], obb_ids[j], quads, transforms, colliders, dynamic_bodies);
	//	}
	//}

	//// Check OBB-SPHERE intersections
	//for (auto id : obb_ids)
	//{
	//	for (auto id2 : sphere_ids)
	//	{
	//		if (id == id2) continue;
	//		intersectsObbInSphere(id, id2, quads, spheres, transforms, colliders, dynamic_bodies);
	//	}
	//}
	////sphere-sphere
	//for (int i = 0; i < sphere_ids.size(); ++i)
	//{
	//	for (int j = i + 1; j < sphere_ids.size(); ++j)
	//	{
	//		intersectsSphereInSphere(sphere_ids[i], sphere_ids[j], spheres, transforms, colliders, dynamic_bodies);
	//	}
	//}
	
}

bool SCollision::pointInAabb(glm::vec3 min, glm::vec3 max, glm::vec3 position, glm::vec3 point)
{
	// Calculate the local coordinates of the point relative to the AABB
	glm::vec3 local_point = point - position;

	// Check if the local coordinates are within the bounds of the AABB
	return (local_point.x >= min.x && local_point.x <= max.x) &&
		(local_point.y >= min.y && local_point.y <= max.y) &&
		(local_point.z >= min.z && local_point.z <= max.z);
}

bool SCollision::intersectsAabbInAabb(const glm::vec3 & min1, const glm::vec3 & max1, const glm::vec3 & min2, const glm::vec3 & max2)
{
	// Check for overlap along the x-axis
	bool overlap_x = (min1.x <= max2.x) && (max1.x >= min2.x);

	// Check for overlap along the y-axis
	bool overlap_y = (min1.y <= max2.y) && (max1.y >= min2.y);

	// Check for overlap along the z-axis
	bool overlap_z = (min1.z <= max2.z) && (max1.z >= min2.z);

	// If there is overlap along all three axes, the AABBs intersect
	return overlap_x && overlap_y && overlap_z;
}

bool SCollision::intersectsSphereInSphere(unsigned id1, unsigned id2, SparseSet<CSphereBounds>& spheres, SparseSet<CTransform>& transforms, SparseSet<CCollider>& colliders,SparseSet<CDynamicBody>& dynamic_bodies)
{
	auto& sphere1 = spheres.get_item(id1);
	auto& sphere2 = spheres.get_item(id2);
	auto& transform1 = transforms.get_item(id1);
	auto& transform2 = transforms.get_item(id2);
	auto& collider1 = colliders.get_item(id1);
	auto& collider2 = colliders.get_item(id2);
	bool sphere1_is_dynamic = dynamic_bodies.hasItem(id1);
	bool sphere2_is_dynamic = dynamic_bodies.hasItem(id2);

	if ((!sphere1_is_dynamic && !sphere2_is_dynamic) ||
		(collider1.collision_bitmask & collider2.collision_bitmask) != 0)
	{
		EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionNotDetectedEvent(id1, id2));
		return false;
	}

	const float distance_squared = glm::length2(transform1.position - transform2.position);
	const float radius_sum = (sphere1.radius * (transform1.scale.x + transform1.scale.y + transform1.scale.z) / 3) +
						 (sphere2.radius * (transform2.scale.x + transform2.scale.y + transform2.scale.z) / 3);

	if (distance_squared > (radius_sum*radius_sum))
	{
		EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionNotDetectedEvent(id1, id2));
		return false;
	}

	float penetration_depth = radius_sum - glm::length(transform1.position - transform2.position);
	glm::vec3 penetration_axis = glm::normalize(transform2.position - transform1.position);

	if (penetration_depth >= 0)
	{
		if(collider1.is_trigger || collider2.is_trigger)
		{
			penetration_depth = 0.0f;
		}
		CollisionManifold manifold{penetration_axis, penetration_depth, transform1, transform2, (sphere1_is_dynamic) ? &dynamic_bodies.get_item(id1) : nullptr,(sphere2_is_dynamic) ? &dynamic_bodies.get_item(id2) : nullptr};
		//std::cout << id1 << " " << id2 << std::endl;
		EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionDetectedEvent(manifold, id1, id2));
	}

	return true;
}

bool SCollision::intersectsObbInObb(const unsigned id1, const unsigned id2, SparseSet<CBoxBounds>& quads, SparseSet<CTransform>& transforms, SparseSet<CCollider>& colliders,SparseSet<CDynamicBody>& dynamic_bodies)
{

	auto& obb1 = quads.get_item(id1);
	auto& obb2 = quads.get_item(id2);
	auto& transform1 = transforms.get_item(id1);
	auto& transform2 = transforms.get_item(id2);
	auto& collider1 = colliders.get_item(id1);
	auto& collider2 = colliders.get_item(id2);
	bool obb1_is_dynamic = dynamic_bodies.hasItem(id1);
	bool obb2_is_dynamic = dynamic_bodies.hasItem(id2);


	if( !obb1_is_dynamic && !obb2_is_dynamic )
	{
		EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionNotDetectedEvent(id1, id2));
		return false;
	}
	const auto object_bitmask = collider1.collision_bitmask;
	const auto other_object_bitmask = collider2.collision_bitmask;
	if( (object_bitmask & other_object_bitmask) != 0 )
	{
		EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionNotDetectedEvent(id1, id2));
		return false;
	}
	// Extract rotation matrices
	const glm::mat4 rotation_matrix1 = glm::eulerAngleXYZ(transform1.rotation.x, transform1.rotation.y, transform1.rotation.z);
	const glm::mat4 rotation_matrix2 = glm::eulerAngleXYZ(transform2.rotation.x, transform2.rotation.y, transform2.rotation.z);

	const auto vertices1 = getObbPointsInWorldSpace(obb1, transform1);
	const auto vertices2 = getObbPointsInWorldSpace(obb2, transform2);

	// Extract axes
	glm::vec3 axes1[3] = {
		rotation_matrix1[0],
		rotation_matrix1[1],
		rotation_matrix1[2]
	};

	glm::vec3 axes2[3] = {
		rotation_matrix2[0],
		rotation_matrix2[1],
		rotation_matrix2[2]
	};

	float min_penetration_depth = std::numeric_limits<float>::max();
	glm::vec3 penetration_axis;

	// Check for intersection using SAT
	for( const auto & axis : axes1 )
	{
		if( ! testAxis(axis, vertices1, vertices2, min_penetration_depth, penetration_axis) ||
			! testAxis(axis * -1.0f, vertices1, vertices2, min_penetration_depth, penetration_axis) )
		{
			EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionNotDetectedEvent(id1, id2));
			return false;
		}
	}

	for( const auto & axis : axes2 )
	{
		if( ! testAxis(axis, vertices1, vertices2, min_penetration_depth, penetration_axis) ||
			! testAxis(axis * -1.0f, vertices1, vertices2, min_penetration_depth, penetration_axis) )
		{
			EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionNotDetectedEvent(id1, id2));
			return false;
		}
	}

	for( const auto & axis1 : axes1 )
	{
		for( const auto & axis2 : axes2 )
		{
			glm::vec3 axis = cross(axis1, axis2);
			if( ! testAxis(axis, vertices1, vertices2, min_penetration_depth, penetration_axis) ||
				! testAxis(axis * -1.0f, vertices1, vertices2, min_penetration_depth, penetration_axis) )
			{
				EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionNotDetectedEvent(id1,id2));
				return false;
			}
		}
	}

	if( min_penetration_depth > 0 )
	{
		if(collider1.is_trigger || collider2.is_trigger)
		{
			min_penetration_depth = 0.0f;
		}
		CollisionManifold manifold{penetration_axis, min_penetration_depth, transform1, transform2, (obb1_is_dynamic) ? &dynamic_bodies.get_item(id1) : nullptr, (obb2_is_dynamic) ? &dynamic_bodies.get_item(id2) : nullptr};
		EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionDetectedEvent(manifold, id1, id2));
	}

	return true;
}

std::vector<glm::vec3> SCollision::getObbPointsInWorldSpace(const CBoxBounds & obb, const CTransform & transform)
{
	// Extract rotation matrix from the transform
	const glm::mat4 rotation_matrix = glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);

	// Compute local OBB points from obb.extents
	std::vector<glm::vec3> local_points = {
		glm::vec3(-obb.extents.x, -obb.extents.y, -obb.extents.z),
		glm::vec3(obb.extents.x, -obb.extents.y, -obb.extents.z),
		glm::vec3(obb.extents.x, obb.extents.y, -obb.extents.z),
		glm::vec3(-obb.extents.x, obb.extents.y, -obb.extents.z),
		glm::vec3(-obb.extents.x, -obb.extents.y, obb.extents.z),
		glm::vec3(obb.extents.x, -obb.extents.y, obb.extents.z),
		glm::vec3(obb.extents.x, obb.extents.y, obb.extents.z),
		glm::vec3(-obb.extents.x, obb.extents.y, obb.extents.z)
	};

	// Transform local points to world space
	std::vector<glm::vec3> world_points;
	world_points.reserve(local_points.size());
	for( const auto & point : local_points )
	{
		glm::vec3 scaled_point = point * transform.scale;
		glm::vec3 rotated_point = rotation_matrix * glm::vec4(scaled_point, 1);
		glm::vec3 world_point = rotated_point + transform.position;
		world_points.emplace_back(world_point);
	}

	return world_points;
}

bool SCollision::testAxis(const glm::vec3 & axis, const std::vector<glm::vec3> & points1, const std::vector<glm::vec3> & points2, float & overlap,
	glm::vec3 & penetration_axis)
{
	if( length2(axis) < 0.00001f )
	{
		return true;
	}

	float min1 = std::numeric_limits<float>::max();
	float max1 = std::numeric_limits<float>::lowest();
	float min2 = std::numeric_limits<float>::max();
	float max2 = std::numeric_limits<float>::lowest();

	for( const auto & point : points1 )
	{
		float projection = dot(point, axis);
		if( projection < min1 ) min1 = projection;
		if( projection > max1 ) max1 = projection;
	}

	for( const auto & point : points2 )
	{
		float projection = dot(point, axis);
		if( projection < min2 ) min2 = projection;
		if( projection > max2 ) max2 = projection;
	}

	float axis_overlap = std::min(max1, max2) - std::max(min1, min2);
	if( axis_overlap < 0 )
	{
		return false;
	}

	if( axis_overlap < overlap )
	{
		overlap = axis_overlap;
		penetration_axis = axis;
		if( min2 < min1 )
		{
			penetration_axis = axis * -1.0f;
		}
	}
	return true;
}

bool SCollision::intersectsObbInSphere(unsigned quad_id, unsigned sphere_id, SparseSet<CBoxBounds>& quads, SparseSet<CSphereBounds>& spheres, SparseSet<CTransform>& transforms, SparseSet<CCollider>& colliders,SparseSet<CDynamicBody>& dynamic_bodies)
{
	auto& obb = quads.get_item(quad_id);
	auto& sphere = spheres.get_item(sphere_id);
	auto& obb_transform = transforms.get_item(quad_id);
	auto& sphere_transform = transforms.get_item(sphere_id);
	auto& obb_collider = colliders.get_item(quad_id);
	auto& sphere_collider = colliders.get_item(sphere_id);
	bool obb_is_dynamic = dynamic_bodies.hasItem(quad_id);
	bool sphere_is_dynamic = dynamic_bodies.hasItem(sphere_id);

	if ((!obb_is_dynamic && !sphere_is_dynamic) ||
		(obb_collider.collision_bitmask & sphere_collider.collision_bitmask) != 0)
	{
		EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionNotDetectedEvent(quad_id, sphere_id));
		return false;
	}

	const glm::mat4 rotation_matrix_obb = glm::eulerAngleXYZ(obb_transform.rotation.x, obb_transform.rotation.y, obb_transform.rotation.z);

	glm::vec3 local_center_sphere = glm::transpose(rotation_matrix_obb) * glm::vec4(sphere_transform.position - obb_transform.position, 1.0f);

	glm::vec3 closest_point = glm::clamp(local_center_sphere, -obb.extents, obb.extents);

	glm::vec3 difference = local_center_sphere - closest_point;
	float penetration_depth = sphere.radius*(sphere_transform.scale.x + sphere_transform.scale.y + sphere_transform.scale.z /3) - glm::length(difference);
	if (penetration_depth < 0)
	{
		EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionNotDetectedEvent(quad_id, sphere_id));
		return false;
	}

	glm::vec3 penetration_axis = glm::normalize(rotation_matrix_obb * glm::vec4(difference, 1.0f));

	if(obb_collider.is_trigger || sphere_collider.is_trigger)
	{
		penetration_depth = 0.0f;
	}
	CollisionManifold manifold{penetration_axis, penetration_depth, obb_transform, sphere_transform, (obb_is_dynamic) ? &dynamic_bodies.get_item(quad_id) : nullptr, (sphere_is_dynamic) ? &dynamic_bodies.get_item(sphere_id) : nullptr};
	EventHandler::GetInstance()->collisionDispatcher.SendEvent(CollisionDetectedEvent(manifold, quad_id, sphere_id));
	return true;
}
