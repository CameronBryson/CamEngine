#pragma once
#include <Registry.hpp>
#include "Components.hpp"

class SCollision {
public:
    static void Update(Registry& registry);
private:
    static bool intersectsAABBInAABB(const CAABB& aabb1, const CAABB& aabb2, const CTransform& transform1, const CTransform& transform2);
    static bool intersectsPointInAABB(const Vec3& point, const CAABB& aabb, const CTransform& aabbTransform);
    static bool intersectsSphereInAABB(const CSphere& sphere, const CAABB& aabb, const CTransform& sphereTransform, const CTransform& aabbTransform);
    static bool intersectsSphereInSphere(const CSphere& sphere1, const CSphere& sphere2, const CTransform& transform1, const CTransform& transform2);
};
