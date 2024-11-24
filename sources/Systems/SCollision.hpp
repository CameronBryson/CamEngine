#pragma once
#include "Engine/ComponentEvents.hpp"
#include "Engine/SparseSet.hpp"
#include "glm/fwd.hpp"
#include "Math/Octree.hpp"
#include "Math/BoundngBox.hpp"
class BaseScene;
struct CSphereBounds;
struct CBoxBounds;
struct CTransform;
struct CCollider;
struct CDynamicBody;

class SCollision
{
public:
	SCollision(BaseScene* scene);
	void init();
	void update();
	void shutdown();
private:
	bool pointInAabb(const glm::vec3& min, const glm::vec3& max, const glm::vec3& position, const glm::vec3& point);
	bool intersectsAabbInAabb(const glm::vec3& min1, const glm::vec3& max1, const glm::vec3& min2,
										const glm::vec3& max2);
	bool intersectsSphereInSphere(unsigned short id1, unsigned short id2, SparseSet<CSphereBounds>& spheres, SparseSet<CTransform>& transforms, SparseSet<CCollider>& colliders, SparseSet<CDynamicBody>& dynamic_bodies);
	bool intersectsObbInObb(unsigned short id1, unsigned short id2, SparseSet<CBoxBounds>& quads, SparseSet<CTransform>& transforms, SparseSet<CCollider>& colliders, SparseSet<CDynamicBody>& dynamic_bodies);
	bool intersectsObbInSphere(unsigned short quad_id, unsigned short sphere_id, SparseSet<CBoxBounds>& quads, SparseSet<CSphereBounds>& spheres, SparseSet<CTransform>& transforms, SparseSet<CCollider>& colliders, SparseSet<CDynamicBody>& dynamic_bodies);
	std::vector<glm::vec3> getObbPointsInWorldSpace(const CBoxBounds& obb, const CTransform& transform);
	bool testAxis(const glm::vec3& axis, const std::vector<glm::vec3>& points1,
						  const std::vector<glm::vec3>& points2, float overlap,  glm::vec3& penetration_axis);
public:
	void OnComponentAdded(const Event<ComponentEvents>& event);
	void OnComponentRemoved(const Event<ComponentEvents>& event);
private:
	//maybe move octree to collision manager
	BoundingBox mSceneBounds;
	Octree mOctree;
	BaseScene* mScene;
};
