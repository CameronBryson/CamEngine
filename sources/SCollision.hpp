#ifndef SCOLLISION_HPP
#define SCOLLISION_HPP
#include <Registry.hpp>
#include "Components.hpp"

class SCollision {
public:
    static void Update(Registry& registry);
private:
    static void checkAABBAABBIntersections(Registry& registry, const std::vector<unsigned short>& AABB_IDS);
    static void checkSphereAABBIntersections(Registry& registry, const std::vector<unsigned short>& AABB_IDS, const std::vector<unsigned short>& Sphere_IDS);
    static void checkSphereSphereIntersections(Registry& registry, const std::vector<unsigned short>& Sphere_IDS);

    static Vec3 GetMin(const CTransform& transform, const CAABB& aabb);
    static Vec3 GetMax(const CTransform& transform, const CAABB& aabb);
    static bool intersectsAABBInAABB(const CAABB& aabb1, const CAABB& aabb2, const CTransform& transform1, const CTransform& transform2);
    static bool intersectsPointInAABB(const Vec3& point, const CAABB& aabb, const CTransform& aabbTransform);
    static bool intersectsSphereInAABB(const CSphere& sphere, const CAABB& aabb, const CTransform& sphereTransform, const CTransform& aabbTransform);
    static bool intersectsSphereInSphere(const CSphere& sphere1, const CSphere& sphere2, const CTransform& transform1, const CTransform& transform2);
};

#endif //SCOLLISION_HPP