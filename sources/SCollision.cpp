#include "SCollision.hpp"

void s_collision::update(const registry &registry)
{
    auto &aabbs = registry.get_sparse_set<c_aabb>();
    auto &transforms = registry.get_sparse_set<c_transform>();
    auto &spheres = registry.get_sparse_set<c_sphere>();

    std::vector<unsigned short> aabb_ids = registry.get_entity_ids<c_aabb, c_transform>();
    std::vector<unsigned short> sphere_ids = registry.get_entity_ids<c_sphere, c_transform>();

    // Check AABB-AABB intersections
    for (const auto id : aabb_ids)
    {
        auto &aabb = aabbs.get_item(id);
        auto &transform = transforms.get_item(id);

        for (const auto inner_id : aabb_ids)
        {
            if (id == inner_id)
                continue;

            if (intersects_aabb_in_aabb(aabb, aabbs.get_item(inner_id), transform, transforms.get_item(inner_id)))
            {
                // printf("AABB %d intersects AABB %d\n", ID, innerID);
            }
        }
    }

    // Check Sphere-AABB intersections
    for (const auto id : aabb_ids)
    {
        auto &aabb = aabbs.get_item(id);
        auto &transform = transforms.get_item(id);

        for (const auto sphere_id : sphere_ids)
        {
            if (intersects_sphere_in_aabb(spheres.get_item(sphere_id), aabb, transforms.get_item(sphere_id), transform))
            {
                // printf("Sphere %d intersects AABB %d\n", sphereID, ID);
            }
        }
    }

    // Check Sphere-Sphere intersections
    for (const auto id : sphere_ids)
    {
        auto &sphere = spheres.get_item(id);
        auto &transform = transforms.get_item(id);

        for (const auto inner_id : sphere_ids)
        {
            if (id == inner_id)
                continue;

            if (intersects_sphere_in_sphere(sphere, spheres.get_item(inner_id), transform, transforms.get_item(inner_id)))
            {
                // printf("Sphere %d intersects Sphere %d\n", ID, innerID);
            }
        }
    }
}

bool s_collision::intersects_aabb_in_aabb(const c_aabb &aabb1, const c_aabb &aabb2, const c_transform &transform1,
                                      const c_transform &transform2)
{
    const vec3 center1 = transform1.position;
    const vec3 center2 = transform2.position;
    const vec3 extents1 = aabb1.extents;
    const vec3 extents2 = aabb2.extents;

    return (std::abs(center1.x - center2.x) <= (extents1.x + extents2.x)) &&
           (std::abs(center1.y - center2.y) <= (extents1.y + extents2.y)) &&
           (std::abs(center1.z - center2.z) <= (extents1.z + extents2.z));
}

bool s_collision::intersects_point_in_aabb(const vec3 &point, const c_aabb &aabb, const c_transform &aabb_transform)
{
    const vec3 center = aabb_transform.position;
    const vec3 extents = aabb.extents;

    return (std::abs(point.x - center.x) <= extents.x) && (std::abs(point.y - center.y) <= extents.y) &&
           (std::abs(point.z - center.z) <= extents.z);
}

bool s_collision::intersects_sphere_in_aabb(const c_sphere &sphere, const c_aabb &aabb, const c_transform &sphere_transform,
                                        const c_transform &aabb_transform)
{
    const vec3 center_aabb = aabb_transform.position;
    const vec3 extents_aabb = aabb.extents;
    const vec3 center_sphere = sphere_transform.position;
    const float radius_sphere = sphere.radius;

    return (std::abs(center_sphere.x - center_aabb.x) <= (extents_aabb.x + radius_sphere)) &&
           (std::abs(center_sphere.y - center_aabb.y) <= (extents_aabb.y + radius_sphere)) &&
           (std::abs(center_sphere.z - center_aabb.z) <= (extents_aabb.z + radius_sphere));
}

bool s_collision::intersects_sphere_in_sphere(const c_sphere &sphere1, const c_sphere &sphere2, const c_transform &transform1,
                                          const c_transform &transform2)
{
    const vec3 center1 = transform1.position;
    const vec3 center2 = transform2.position;
    const float radius1 = sphere1.radius;
    const float radius2 = sphere2.radius;

    const float distance_squared = (center1 - center2).length_squared();
    const float radius_sum = radius1 + radius2;

    return distance_squared <= (radius_sum * radius_sum);
}