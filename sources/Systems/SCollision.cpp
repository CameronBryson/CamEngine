#include "SCollision.hpp"

#include "../Engine/Collision.hpp"

#include <glm/gtx/norm.hpp>
#include <glm/gtx/euler_angles.hpp>

void s_collision::update(registry &registry)
{
    auto &colliders = registry.get_sparse_set<c_collider>();
    auto &obbs = registry.get_sparse_set<c_quad>();
    auto &transforms = registry.get_sparse_set<c_transform>();
    auto &spheres = registry.get_sparse_set<c_sphere>();

    std::vector<unsigned short> obb_ids = registry.get_entity_ids<c_quad, c_transform, c_collider>();
    std::vector<unsigned short> sphere_ids = registry.get_entity_ids<c_sphere, c_transform, c_collider>();

    // Check OBB-OBB intersections
    for (auto id : obb_ids)
    {
        auto &obb = obbs.get_item(id);
        auto &transform = transforms.get_item(id);
        auto &collider = colliders.get_item(id);

        for (auto id2 : obb_ids)
        {
            if(id == id2) continue;
            auto &inner_obb = obbs.get_item(id2);
            auto &inner_transform = transforms.get_item(id2);
            auto &inner_collider = colliders.get_item(id2);
            if (intersects_obb_in_obb(obb, inner_obb, transform, inner_transform,collider,inner_collider, registry))
            {

                // printf("OBB %d intersects OBB%d\n", obb_ids[i], obb_ids[j]);
            }
        }
    }

    // Check OBB-SPHERE intersections
    for (auto id : obb_ids)
    {
        auto &obb = obbs.get_item(id);
        auto &obb_transform = transforms.get_item(id);
        auto &obb_collider = colliders.get_item(id);

        for (auto id2 : sphere_ids)
        {
            if(id==id2) continue;
            auto &sphere = spheres.get_item(id2);
            auto &sphere_transform = transforms.get_item(id2);
            auto &sphere_collider = colliders.get_item(id2);
            if (intersects_obb_in_sphere(obb, sphere, obb_transform,
                                         sphere_transform,obb_collider,sphere_collider, registry))
            {
                // printf("OBB %d intersects Sphere %d\n", i, j);
            }
        }
    }
    //sphere-sphere
    for (auto id : sphere_ids)
    {
        auto &sphere = spheres.get_item(id);
        auto &sphere_transform = transforms.get_item(id);
        auto &sphere_collider = colliders.get_item(id);
        for (auto id2 : sphere_ids)
        {
            auto &sphere2 = spheres.get_item(id2);
            auto &sphere2_transform = transforms.get_item(id2);
            auto &sphere2_collider = colliders.get_item(id2);
            if (intersects_sphere_in_sphere(sphere, sphere2, sphere_transform, sphere2_transform, sphere_collider, sphere2_collider, registry))
            {
                 printf("Sphere %d intersects SPHERE %d\n", id, id2);
            }
        }
    }
}
bool s_collision::point_in_aabb(glm::vec3 min, glm::vec3 max, glm::vec3 position, glm::vec3 point)
{
    // Calculate the local coordinates of the point relative to the AABB
    glm::vec3 local_point = point - position;

    // Check if the local coordinates are within the bounds of the AABB
    return (local_point.x >= min.x && local_point.x <= max.x) &&
           (local_point.y >= min.y && local_point.y <= max.y) &&
           (local_point.z >= min.z && local_point.z <= max.z);
}
bool s_collision::intersects_aabb_in_aabb(const glm::vec3 &min1, const glm::vec3 &max1, const glm::vec3 &min2, const glm::vec3 &max2)
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
bool s_collision::intersects_sphere_in_sphere(const c_sphere &sphere1, const c_sphere &sphere2, c_transform &transform1,
                                              c_transform &transform2, c_collider &collider1, c_collider &collider2,
                                              registry &registry)
{
    const object_collision_type type1 = collider1.collision_type;
    const object_collision_type type2 = collider2.collision_type;
    if(type1 == object_collision_type::STATIC && type2 == object_collision_type::STATIC)
    {
        return false;
    }
    const auto object_bitmask = collider1.collision_bitmask;
    const auto other_object_bitmask = collider2.collision_bitmask;
    if ((object_bitmask & other_object_bitmask) == 0)
    {
        return false;
    }
    const glm::vec3 center1 = transform1.position;
    const glm::vec3 center2 = transform2.position;
    const float radius1 = sphere1.radius;
    const float radius2 = sphere2.radius;
    const float distance_squared = glm::length2(center1 - center2);
    const float radius_sum = radius1 + radius2;

    if (distance_squared > (radius_sum * radius_sum)) {
        return false;
    }

    float distance = std::sqrt(distance_squared);
    float penetration_depth = radius_sum - distance;
    glm::vec3 penetration_axis = normalize((center2 - center1));

    if(penetration_depth > 0.000001)
    {
        registry.add_collision_manifold({penetration_axis, penetration_depth, transform1, transform2, type1, type2});
    }
    return true;
}
bool s_collision::intersects_obb_in_obb(const c_quad &obb1, const c_quad &obb2, c_transform &transform1,
                                        c_transform &transform2, c_collider &collider1, c_collider &collider2,
                                        registry &registry) {
    const object_collision_type type1 = collider1.collision_type;
    const object_collision_type type2 = collider2.collision_type;
    if(type1 == object_collision_type::STATIC && type2 == object_collision_type::STATIC)
    {
        return false;
    }
    const auto object_bitmask = collider1.collision_bitmask;
    const auto other_object_bitmask = collider2.collision_bitmask;
    if ((object_bitmask & other_object_bitmask) == 0)
    {
        return false;
    }
    // Extract rotation matrices
    const glm::mat4 rotation_matrix1 = glm::eulerAngleXYZ(transform1.rotation.x,transform1.rotation.y,transform1.rotation.z);
    const glm::mat4 rotation_matrix2= glm::eulerAngleXYZ(transform2.rotation.x,transform2.rotation.y,transform2.rotation.z);

    const auto vertices1 = get_obb_points_in_world_space(obb1, transform1);
    const auto vertices2 = get_obb_points_in_world_space(obb2, transform2);

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
    for (const auto &axis : axes1) {
        if (!test_axis(axis, vertices1, vertices2, min_penetration_depth,penetration_axis)||
            !test_axis(axis*-1.0f, vertices1, vertices2, min_penetration_depth,penetration_axis))
            return false;
    }

    for (const auto &axis : axes2) {
        if (!test_axis(axis, vertices1, vertices2, min_penetration_depth,penetration_axis)||
            !test_axis(axis*-1.0f, vertices1, vertices2, min_penetration_depth,penetration_axis))
            return false;
    }

    for (const auto &axis1 : axes1) {
        for (const auto &axis2 : axes2) {
            glm::vec3 axis = cross(axis1, axis2);
            if (!test_axis(axis, vertices1, vertices2, min_penetration_depth,penetration_axis)||
               !test_axis(axis*-1.0f, vertices1, vertices2, min_penetration_depth,penetration_axis))
                return false;
        }
    }

    if (min_penetration_depth > 0.000001) {
        registry.add_collision_manifold({penetration_axis, min_penetration_depth, transform1, transform2, type1, type2});
    }

    return true;
}
std::vector<glm::vec3> s_collision::get_obb_points_in_world_space(const c_quad &obb, const c_transform &transform)
{
    // Extract rotation matrix from the transform
    const glm::mat4 rotation_matrix = glm::eulerAngleXYZ(transform.rotation.x,transform.rotation.y,transform.rotation.z);

    // Compute local OBB points from obb.extents
    std::vector<glm::vec3> local_points = {
        glm::vec3(-obb.extents.x, -obb.extents.y, -obb.extents.z),
        glm::vec3( obb.extents.x, -obb.extents.y, -obb.extents.z),
        glm::vec3( obb.extents.x,  obb.extents.y, -obb.extents.z),
        glm::vec3(-obb.extents.x,  obb.extents.y, -obb.extents.z),
        glm::vec3(-obb.extents.x, -obb.extents.y,  obb.extents.z),
        glm::vec3( obb.extents.x, -obb.extents.y,  obb.extents.z),
        glm::vec3( obb.extents.x,  obb.extents.y,  obb.extents.z),
        glm::vec3(-obb.extents.x,  obb.extents.y,  obb.extents.z)
    };

    // Transform local points to world space
    std::vector<glm::vec3> world_points;
    world_points.reserve(local_points.size());
    for (const auto &point : local_points)
    {
         glm::vec3 scaled_point = point * transform.scale;
         glm::vec3 rotated_point = rotation_matrix * glm::vec4(scaled_point,1);
         glm::vec3 world_point = rotated_point + transform.position;
        world_points.push_back(world_point);
    }

    return world_points;
}
bool s_collision::test_axis(const glm::vec3 &axis, const std::vector<glm::vec3> &points1, const std::vector<glm::vec3> &points2, float &overlap, glm::vec3 &penetration_axis) {
     if (length2(axis) < 0.00001f) {
         return true;
     }

    float min1 = std::numeric_limits<float>::max();
    float max1 = std::numeric_limits<float>::lowest();
    float min2 = std::numeric_limits<float>::max();
    float max2 = std::numeric_limits<float>::lowest();

    for (const auto &point : points1) {
        float projection = dot(point, axis);
        if (projection < min1) min1 = projection;
        if (projection > max1) max1 = projection;
    }

    for (const auto &point : points2) {
        float projection = dot(point, axis);
        if (projection < min2) min2 = projection;
        if (projection > max2) max2 = projection;
    }

    float axis_overlap = std::min(max1, max2) - std::max(min1, min2);
    if (axis_overlap < 0) {
        return false;
    }

    if (axis_overlap < overlap) {
        overlap = axis_overlap;
        penetration_axis = axis;
        if(min2 > min1)
        {
            penetration_axis = axis*-1.0f;
        }
    }
    return true;
}
bool s_collision::intersects_obb_in_sphere(const c_quad &obb, c_sphere &sphere, c_transform &obb_transform,
                                           c_transform &sphere_transform, c_collider &obb_collider,
                                           c_collider &sphere_collider, registry &registry) {
    const object_collision_type type1 = obb_collider.collision_type;
    const object_collision_type type2 = sphere_collider.collision_type;
    if(type1 == object_collision_type::STATIC && type2 == object_collision_type::STATIC)
    {
        return false;
    }
    const auto object_bitmask = obb_collider.collision_bitmask;
    const auto other_object_bitmask = sphere_collider.collision_bitmask;
    if ((object_bitmask & other_object_bitmask) == 0)
    {
        return false;
    }
    // Extract OBB data
    const glm::vec3 center_obb = obb_transform.position;
    const glm::vec3 extents_obb = obb.extents * 0.5f;
    const glm::mat4 rotation_matrix_obb = glm::eulerAngleXYZ(obb_transform.rotation.x,obb_transform.rotation.y,obb_transform.rotation.z);

    // Extract sphere data
    const glm::vec3 center_sphere = sphere_transform.position;
    const float radius_sphere = sphere.radius;

    // Transform sphere center to OBB local space
    glm::vec3 local_center_sphere = (glm::transpose(rotation_matrix_obb) * glm::vec4(center_sphere-center_obb,1));

    // Find the closest point on the OBB to the sphere center
    glm::vec3 closest_point = local_center_sphere;
    closest_point.x = std::max(-extents_obb.x, std::min(local_center_sphere.x, extents_obb.x));
    closest_point.y = std::max(-extents_obb.y, std::min(local_center_sphere.y, extents_obb.y));
    closest_point.z = std::max(-extents_obb.z, std::min(local_center_sphere.z, extents_obb.z));

    // Compute the vector from the sphere center to the closest point
    glm::vec3 difference = local_center_sphere - closest_point;
    float distance_squared = glm::length2(difference);

    // Check if the distance is less than the sphere radius
    if (distance_squared > radius_sphere * radius_sphere) {
        return false;
    }

    float distance = std::sqrt(distance_squared);
    float penetration_depth = radius_sphere - distance;
    glm::vec3 penetration_axis = glm::normalize(rotation_matrix_obb * glm::vec4(difference,1));
    if(penetration_depth > 0.000001)
    {
        registry.add_collision_manifold({penetration_axis, penetration_depth, obb_transform, sphere_transform, type1, type2});
    }
    // Adjust positions for penetration resolution

    return true;
}
