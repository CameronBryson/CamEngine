#include "SCollision.hpp"

#include "Faces.hpp"
#include "Matrix.hpp"
#include "Collision.hpp"

void s_collision::update(registry &registry)
{
    auto &colliders = registry.get_sparse_set<c_collider>();
    auto &obbs = registry.get_sparse_set<c_quad>();
    auto &transforms = registry.get_sparse_set<c_transform>();
    auto &spheres = registry.get_sparse_set<c_sphere>();

    std::vector<unsigned short> obb_ids = registry.get_entity_ids<c_quad, c_transform, c_collider>();
    std::vector<unsigned short> sphere_ids = registry.get_entity_ids<c_sphere, c_transform, c_collider>();

    // Check OBB-OBB intersections
    for (size_t i = 0; i < obb_ids.size(); ++i)
    {
        auto &obb = obbs.get_item(obb_ids[i]);
        auto &transform = transforms.get_item(obb_ids[i]);
        auto &collider = colliders.get_item(obb_ids[i]);

        for (size_t j = i+1; j < obb_ids.size(); ++j)
        {
            auto &inner_obb = obbs.get_item(obb_ids[j]);
            auto &inner_transform = transforms.get_item(obb_ids[j]);
            auto &inner_collider = colliders.get_item(obb_ids[j]);
            if (intersects_obb_in_obb(obb, inner_obb, transform, inner_transform,collider,inner_collider, registry))
            {

                // printf("OBB %d intersects OBB%d\n", obb_ids[i], obb_ids[j]);
            }
        }
    }

    // Check Sphere-AABB intersections
    for (size_t i = 0; i < obb_ids.size(); ++i)
    {
        auto &obb = obbs.get_item(obb_ids[i]);
        auto &obb_transform = transforms.get_item(obb_ids[i]);
        auto &obb_collider = colliders.get_item(obb_ids[i]);

        for (size_t j = i+1; j < sphere_ids.size(); ++j)
        {
            auto &sphere = spheres.get_item(sphere_ids[j]);
            auto &sphere_transform = transforms.get_item(sphere_ids[j]);
            auto &sphere_collider = colliders.get_item(sphere_ids[j]);
            if (intersects_obb_in_sphere(obb, sphere, obb_transform,
                                         sphere_transform,obb_collider,sphere_collider, registry))
            {
                // printf("OBB %d intersects Sphere %d\n", i, j);
            }
        }
    }

    for (size_t i = 0; i < sphere_ids.size(); ++i)
    {
        auto &sphere = spheres.get_item(sphere_ids[i]);
        auto &sphere_transform = transforms.get_item(sphere_ids[i]);
        auto &sphere_collider = colliders.get_item(sphere_ids[i]);
        for (size_t j = i+1; j < obb_ids.size(); ++j)
        {
            auto &obb = obbs.get_item(obb_ids[j]);
            auto &obb_transform = transforms.get_item(obb_ids[j]);
            auto &obb_collider = colliders.get_item(obb_ids[j]);
            if (intersects_obb_in_sphere(obb, sphere, obb_transform, sphere_transform, obb_collider, sphere_collider, registry))
            {
                // printf("Sphere %d intersects OBB %d\n", i, j);
            }
        }
    }
}
bool s_collision::point_in_aabb(vec3 min, vec3 max, vec3 position, vec3 point)
{
    // Calculate the local coordinates of the point relative to the AABB
    vec3 local_point = point - position;

    // Check if the local coordinates are within the bounds of the AABB
    return (local_point.x >= min.x && local_point.x <= max.x) &&
           (local_point.y >= min.y && local_point.y <= max.y) &&
           (local_point.z >= min.z && local_point.z <= max.z);
}
bool s_collision::intersects_aabb_in_aabb(const vec3 &min1, const vec3 &max1, const vec3 &min2, const vec3 &max2)
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
    const vec3 center1 = transform1.position;
    const vec3 center2 = transform2.position;
    const float radius1 = sphere1.radius;
    const float radius2 = sphere2.radius;

    const float distance_squared = (center1 - center2).length_squared();
    const float radius_sum = radius1 + radius2;

    if (distance_squared > (radius_sum * radius_sum)) {
        return false;
    }

    float distance = std::sqrt(distance_squared);
    float penetration_depth = radius_sum - distance;
    vec3 penetration_axis = (center2 - center1).normalized();

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
    const mat4 rotation_matrix1 = mat4::create_rotation_matrix(transform1.rotation);
    const mat4 rotation_matrix2 = mat4::create_rotation_matrix(transform2.rotation);

    const auto vertices1 = get_obb_points_in_world_space(obb1, transform1);
    const auto vertices2 = get_obb_points_in_world_space(obb2, transform2);

    // Extract axes
    vec3 axes1[3] = {
        rotation_matrix1.get_right_axis(),
        rotation_matrix1.get_up_axis(),
        rotation_matrix1.get_forward_axis(),
    };

    vec3 axes2[3] = {
        rotation_matrix2.get_right_axis(),
        rotation_matrix2.get_up_axis(),
        rotation_matrix2.get_forward_axis()
    };

    float min_penetration_depth = std::numeric_limits<float>::max();
    vec3 penetration_axis;

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
            vec3 axis = axis1.cross_product(axis2);
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
std::vector<vec3> s_collision::get_obb_points_in_world_space(const c_quad &obb, const c_transform &transform)
{
    // Extract rotation matrix from the transform
    mat4 rotation_matrix = mat4::create_rotation_matrix(transform.rotation);

    // Compute local OBB points
    std::vector<vec3> local_points = faces::get_quad_vertices(obb.extents);

    // Transform local points to world space
    std::vector<vec3> world_points;
    world_points.reserve(local_points.size());
    for (const auto &point : local_points)
    {
        vec3 scaled_point = point * transform.scale;
        vec3 rotated_point = rotation_matrix * scaled_point;
        vec3 world_point = rotated_point + transform.position;
        world_points.push_back(world_point);
    }

    return world_points;
}
bool s_collision::test_axis(const vec3 &axis, const std::vector<vec3> &points1, const std::vector<vec3> &points2, float &overlap, vec3 &penetration_axis) {
     if (axis.length_squared() < 0.00001f) {
         return true;
     }

    float min1 = std::numeric_limits<float>::max();
    float max1 = std::numeric_limits<float>::lowest();
    float min2 = std::numeric_limits<float>::max();
    float max2 = std::numeric_limits<float>::lowest();

    for (const auto &point : points1) {
        float projection = point.dot_product(axis);
        if (projection < min1) min1 = projection;
        if (projection > max1) max1 = projection;
    }

    for (const auto &point : points2) {
        float projection = point.dot_product(axis);
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
    const vec3 center_obb = obb_transform.position;
    const vec3 extents_obb = obb.extents * 0.5f;
    const mat4 rotation_matrix_obb = mat4::create_rotation_matrix(obb_transform.rotation);

    // Extract sphere data
    const vec3 center_sphere = sphere_transform.position;
    const float radius_sphere = sphere.radius;

    // Transform sphere center to OBB local space
    vec3 local_center_sphere = rotation_matrix_obb.transpose() * (center_sphere - center_obb);

    // Find the closest point on the OBB to the sphere center
    vec3 closest_point = local_center_sphere;
    closest_point.x = std::max(-extents_obb.x, std::min(local_center_sphere.x, extents_obb.x));
    closest_point.y = std::max(-extents_obb.y, std::min(local_center_sphere.y, extents_obb.y));
    closest_point.z = std::max(-extents_obb.z, std::min(local_center_sphere.z, extents_obb.z));

    // Compute the vector from the sphere center to the closest point
    vec3 difference = local_center_sphere - closest_point;
    float distance_squared = difference.length_squared();

    // Check if the distance is less than the sphere radius
    if (distance_squared > radius_sphere * radius_sphere) {
        return false;
    }

    float distance = std::sqrt(distance_squared);
    float penetration_depth = radius_sphere - distance;
    vec3 penetration_axis = (rotation_matrix_obb * difference).normalized();
    if(penetration_depth > 0.000001)
    {
        registry.add_collision_manifold({penetration_axis, penetration_depth, obb_transform, sphere_transform, type1, type2});
    }
    // Adjust positions for penetration resolution

    return true;
}
