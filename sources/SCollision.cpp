#include "SCollision.hpp"

void SCollision::Update(Registry &registry) {
    std::vector<unsigned short> AABB_IDS = registry.getEntityIDS<CAABB, CTransform>();
    std::vector<unsigned short> Sphere_IDS = registry.getEntityIDS<CSphere, CTransform>();

    checkAABBAABBIntersections(registry, AABB_IDS);
    checkSphereAABBIntersections(registry, AABB_IDS, Sphere_IDS);
    checkSphereSphereIntersections(registry, Sphere_IDS);
}

void SCollision::checkAABBAABBIntersections(Registry& registry, const std::vector<unsigned short>& AABB_IDS) {
    auto& aabbs = registry.getSparseSet<CAABB>();
    auto& transforms = registry.getSparseSet<CTransform>();

    for (auto ID : AABB_IDS) {
        auto& aabb = aabbs.getItem(ID);
        auto& transform = transforms.getItem(ID);

        for (auto innerID : AABB_IDS) {
            if (ID == innerID) continue;

            if (intersectsAABBInAABB(aabb, aabbs.getItem(innerID), transform, transforms.getItem(innerID))) {
                printf("AABB %d intersects AABB %d\n", ID, innerID);
            }
        }
    }
}

void SCollision::checkSphereAABBIntersections(Registry& registry, const std::vector<unsigned short>& AABB_IDS, const std::vector<unsigned short>& Sphere_IDS) {
    auto& aabbs = registry.getSparseSet<CAABB>();
    auto& transforms = registry.getSparseSet<CTransform>();
    auto& spheres = registry.getSparseSet<CSphere>();

    for (auto ID : AABB_IDS) {
        auto& aabb = aabbs.getItem(ID);
        auto& transform = transforms.getItem(ID);

        for (auto sphereID : Sphere_IDS) {
            if (intersectsSphereInAABB(spheres.getItem(sphereID), aabb, transforms.getItem(sphereID), transform)) {
                printf("Sphere %d intersects AABB %d\n", sphereID, ID);
            }
        }
    }
}

void SCollision::checkSphereSphereIntersections(Registry& registry, const std::vector<unsigned short>& Sphere_IDS) {
    auto& transforms = registry.getSparseSet<CTransform>();
    auto& spheres = registry.getSparseSet<CSphere>();

    for (auto ID : Sphere_IDS) {
        auto& sphere = spheres.getItem(ID);
        auto& transform = transforms.getItem(ID);

        for (auto innerID : Sphere_IDS) {
            if (ID == innerID) continue;

            if (intersectsSphereInSphere(sphere, spheres.getItem(innerID), transform, transforms.getItem(innerID))) {
                printf("Sphere %d intersects Sphere %d\n", ID, innerID);
            }
        }
    }
}

Vec3 SCollision::GetMin(const CTransform &transform, const CAABB &aabb) {
    Vec3 halfExtents = aabb.extents * 0.5f;
    return transform.position - halfExtents;
}

Vec3 SCollision::GetMax(const CTransform &transform, const CAABB &aabb) {
    Vec3 halfExtents = aabb.extents * 0.5f;
    return transform.position + halfExtents;
}

bool SCollision::intersectsAABBInAABB(const CAABB &aabb1, const CAABB &aabb2, const CTransform &transform1, const CTransform &transform2) {
    Vec3 min1 = GetMin(transform1, aabb1);
    Vec3 max1 = GetMax(transform1, aabb1);
    Vec3 min2 = GetMin(transform2, aabb2);
    Vec3 max2 = GetMax(transform2, aabb2);

    return (min1.x <= max2.x && max1.x >= min2.x) &&
           (min1.y <= max2.y && max1.y >= min2.y) &&
           (min1.z <= max2.z && max1.z >= min2.z);
}

bool SCollision::intersectsPointInAABB(const Vec3 &point, const CAABB &aabb, const CTransform &aabbTransform) {
    Vec3 aabbMin = GetMin(aabbTransform, aabb);
    Vec3 aabbMax = GetMax(aabbTransform, aabb);

    return (point.x >= aabbMin.x && point.x <= aabbMax.x) &&
           (point.y >= aabbMin.y && point.y <= aabbMax.y) &&
           (point.z >= aabbMin.z && point.z <= aabbMax.z);
}

bool SCollision::intersectsSphereInAABB(const CSphere &sphere, const CAABB &aabb, const CTransform &sphereTransform, const CTransform &aabbTransform) {
    Vec3 aabbMin = GetMin(aabbTransform, aabb);
    Vec3 aabbMax = GetMax(aabbTransform, aabb);

    Vec3 sphereCenter = sphereTransform.position;
    float sphereRadius = sphere.radius;

    return (sphereCenter.x + sphereRadius >= aabbMin.x && sphereCenter.x - sphereRadius <= aabbMax.x) &&
           (sphereCenter.y + sphereRadius >= aabbMin.y && sphereCenter.y - sphereRadius <= aabbMax.y) &&
           (sphereCenter.z + sphereRadius >= aabbMin.z && sphereCenter.z - sphereRadius <= aabbMax.z);
}

bool SCollision::intersectsSphereInSphere(const CSphere &sphere1, const CSphere &sphere2, const CTransform &transform1, const CTransform &transform2) {
    Vec3 center1 = transform1.position;
    Vec3 center2 = transform2.position;
    float radius1 = sphere1.radius;
    float radius2 = sphere2.radius;

    float distanceSquared = (center1 - center2).lengthSquared();
    float radiusSum = radius1 + radius2;

    return distanceSquared <= (radiusSum * radiusSum);
}