#include "SCollision.hpp"

void SCollision::Update(Registry &registry) {
    auto& aabbs = registry.getSparseSet<CAABB>();
    auto& transforms = registry.getSparseSet<CTransform>();
    auto& spheres = registry.getSparseSet<CSphere>();

    std::vector<unsigned short> AABB_IDS = registry.getEntityIDS<CAABB, CTransform>();
    std::vector<unsigned short> Sphere_IDS = registry.getEntityIDS<CSphere, CTransform>();

    // Check AABB-AABB intersections
    for (auto ID : AABB_IDS) {
        auto& aabb = aabbs.getItem(ID);
        auto& transform = transforms.getItem(ID);

        for (auto innerID : AABB_IDS) {
            if (ID == innerID) continue;

            if (intersectsAABBInAABB(aabb, aabbs.getItem(innerID), transform, transforms.getItem(innerID))) {
                //printf("AABB %d intersects AABB %d\n", ID, innerID);
            }
        }
    }

    // Check Sphere-AABB intersections
    for (auto ID : AABB_IDS) {
        auto& aabb = aabbs.getItem(ID);
        auto& transform = transforms.getItem(ID);

        for (auto sphereID : Sphere_IDS) {
            if (intersectsSphereInAABB(spheres.getItem(sphereID), aabb, transforms.getItem(sphereID), transform)) {
                //printf("Sphere %d intersects AABB %d\n", sphereID, ID);
            }
        }
    }

    // Check Sphere-Sphere intersections
    for (auto ID : Sphere_IDS) {
        auto& sphere = spheres.getItem(ID);
        auto& transform = transforms.getItem(ID);

        for (auto innerID : Sphere_IDS) {
            if (ID == innerID) continue;

            if (intersectsSphereInSphere(sphere, spheres.getItem(innerID), transform, transforms.getItem(innerID))) {
                //printf("Sphere %d intersects Sphere %d\n", ID, innerID);
            }
        }
    }
}


bool SCollision::intersectsAABBInAABB(const CAABB &aabb1, const CAABB &aabb2, const CTransform &transform1, const CTransform &transform2) {
    Vec3 center1 = transform1.position;
    Vec3 center2 = transform2.position;
    Vec3 extents1 = aabb1.extents;
    Vec3 extents2 = aabb2.extents;

    return (std::abs(center1.x - center2.x) <= (extents1.x + extents2.x)) &&
           (std::abs(center1.y - center2.y) <= (extents1.y + extents2.y)) &&
           (std::abs(center1.z - center2.z) <= (extents1.z + extents2.z));
}

bool SCollision::intersectsPointInAABB(const Vec3 &point, const CAABB &aabb, const CTransform &aabbTransform) {
    Vec3 center = aabbTransform.position;
    Vec3 extents = aabb.extents;

    return (std::abs(point.x - center.x) <= extents.x) &&
           (std::abs(point.y - center.y) <= extents.y) &&
           (std::abs(point.z - center.z) <= extents.z);
}

bool SCollision::intersectsSphereInAABB(const CSphere &sphere, const CAABB &aabb, const CTransform &sphereTransform, const CTransform &aabbTransform) {
    Vec3 centerAABB = aabbTransform.position;
    Vec3 extentsAABB = aabb.extents;
    Vec3 centerSphere = sphereTransform.position;
    float radiusSphere = sphere.radius;

    return (std::abs(centerSphere.x - centerAABB.x) <= (extentsAABB.x + radiusSphere)) &&
           (std::abs(centerSphere.y - centerAABB.y) <= (extentsAABB.y + radiusSphere)) &&
           (std::abs(centerSphere.z - centerAABB.z) <= (extentsAABB.z + radiusSphere));
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