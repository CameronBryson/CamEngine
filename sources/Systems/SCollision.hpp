#pragma once
#include "Engine/SparseSet.hpp"
#include "glm/glm.hpp"
class Registry;
struct CSphere;
struct CQuad;
struct CTransform;
struct CCollider;
struct CDynamicBody;

class SCollision
{
public:
    void update(Registry& registry);
private:
    bool pointInAabb(glm::vec3 min, glm::vec3 max, glm::vec3 position, glm::vec3 point);
    bool intersectsAabbInAabb(const glm::vec3& min1, const glm::vec3& max1, const glm::vec3& min2,
                                        const glm::vec3& max2);
    bool intersectsSphereInSphere(unsigned id1, unsigned id2, SparseSet<CSphere>& spheres, SparseSet<CTransform>& transforms, SparseSet<CCollider>& colliders, SparseSet<CDynamicBody>& dynamic_bodies);
    bool intersectsObbInObb(unsigned id1, unsigned id2, SparseSet<CQuad>& quads, SparseSet<CTransform>& transforms, SparseSet<CCollider>& colliders, SparseSet<CDynamicBody>& dynamic_bodies);
    bool intersectsObbInSphere(unsigned quad_id, unsigned sphere_id, SparseSet<CQuad>& quads, SparseSet<CSphere>& spheres, SparseSet<CTransform>& transforms, SparseSet<CCollider>& colliders, SparseSet<CDynamicBody>& dynamic_bodies);
    std::vector<glm::vec3> getObbPointsInWorldSpace(const CQuad& obb, const CTransform& transform);
    bool testAxis(const glm::vec3& axis, const std::vector<glm::vec3>& points1,
                          const std::vector<glm::vec3>& points2, float& overlap, glm::vec3& penetration_axis);
};
