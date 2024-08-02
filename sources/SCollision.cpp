#include "SCollision.hpp"

#include "Faces.hpp"
#include "Matrix.hpp"

void s_collision::update(const registry &registry)
{
    auto &aabbs = registry.get_sparse_set<c_quad>();
    auto &transforms = registry.get_sparse_set<c_transform>();
    auto &spheres = registry.get_sparse_set<c_sphere>();

    std::vector<unsigned short> aabb_ids = registry.get_entity_ids<c_quad, c_transform>();
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
            CollisionResult collision = intersects_obb_in_obb(aabb, aabbs.get_item(inner_id), transform, transforms.get_item(inner_id));
            if (collision.intersects)
            {

                printf("AABB %d intersects AABB %d\n", id, inner_id);
                printf("Penetration depth: %f\n", collision.penetration_depth);
                printf("Penetration axis: %f, %f, %f\n", collision.penetration_axis.x, collision.penetration_axis.y, collision.penetration_axis.z);
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
            CollisionResult collision = intersects_obb_in_sphere(aabb,spheres.get_item(sphere_id),transform,transforms.get_item(sphere_id));
            if (collision.intersects)
            {
                printf("Sphere %d intersects AABB %d\n", sphere_id, id);
                printf("Penetration depth: %f\n", collision.penetration_depth);
                printf("Penetration axis: %f, %f, %f\n", collision.penetration_axis.x, collision.penetration_axis.y, collision.penetration_axis.z);

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
            CollisionResult collision = intersects_sphere_in_sphere(sphere, spheres.get_item(inner_id), transform, transforms.get_item(inner_id));
            if (collision.intersects)
            {
                printf("Sphere %d intersects Sphere %d\n", id, inner_id);
                printf("Penetration depth: %f\n", collision.penetration_depth);
                printf("Penetration axis: %f, %f, %f\n", collision.penetration_axis.x, collision.penetration_axis.y, collision.penetration_axis.z);
            }
        }
    }
}

CollisionResult s_collision::intersects_aabb_in_aabb(const c_quad &aabb1, const c_quad &aabb2, c_transform &transform1,
                                                     c_transform &transform2) {
    const vec3 center1 = transform1.position;
    const vec3 center2 = transform2.position;
    const vec3 extents1 = aabb1.extents * 0.5f;
    const vec3 extents2 = aabb2.extents * 0.5f;

    vec3 axes[3] = { vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1) };
    float min_penetration_depth = std::numeric_limits<float>::max();
    vec3 penetration_axis;

    for (const auto &axis : axes) {
        float distance = std::fabs((center1 - center2).dot_product(axis));
        float overlap = (extents1.dot_product(axis) + extents2.dot_product(axis)) - distance;

        if (overlap < 0) {
            return {false, 0, vec3()};
        }

        if (overlap < min_penetration_depth) {
            min_penetration_depth = overlap;
            penetration_axis = axis;
        }
    }

    return {true, min_penetration_depth, penetration_axis};
}


CollisionResult s_collision::intersects_sphere_in_aabb(const c_sphere &sphere, const c_quad &aabb, c_transform &sphere_transform,
                                                       c_transform &aabb_transform) {
    const vec3 center_aabb = aabb_transform.position;
    const vec3 extents_aabb = aabb.extents * 0.5f;
    const vec3 center_sphere = sphere_transform.position;
    const float radius_sphere = sphere.radius;

    // Calculate the closest point on the AABB to the sphere center
    vec3 closest_point = center_sphere;
    closest_point.x = std::max(center_aabb.x - extents_aabb.x, std::min(center_sphere.x, center_aabb.x + extents_aabb.x));
    closest_point.y = std::max(center_aabb.y - extents_aabb.y, std::min(center_sphere.y, center_aabb.y + extents_aabb.y));
    closest_point.z = std::max(center_aabb.z - extents_aabb.z, std::min(center_sphere.z, center_aabb.z + extents_aabb.z));

    // Compute the vector from the sphere center to the closest point
    vec3 difference = center_sphere - closest_point;
    float distance_squared = difference.length_squared();

    // Check if the distance is less than the sphere radius
    if (distance_squared > radius_sphere * radius_sphere) {
        return {false, 0, vec3()};
    }

    float distance = std::sqrt(distance_squared);
    float penetration_depth = radius_sphere - distance;
    vec3 penetration_axis = difference.normalized();
    aabb_transform.position = aabb_transform.position - penetration_axis * (penetration_depth * 0.5f);
    sphere_transform.position = sphere_transform.position + penetration_axis * (penetration_depth * 0.5f);
    return {true, penetration_depth, penetration_axis};
}

CollisionResult s_collision::intersects_sphere_in_sphere(const c_sphere &sphere1, const c_sphere &sphere2,
                                              c_transform &transform1, c_transform &transform2)
{
    const vec3 center1 = transform1.position;
    const vec3 center2 = transform2.position;
    const float radius1 = sphere1.radius;
    const float radius2 = sphere2.radius;

    const float distance_squared = (center1 - center2).length_squared();
    const float radius_sum = radius1 + radius2;

    if (distance_squared > (radius_sum * radius_sum)) {
        return {false, 0, vec3()};
    }

    float distance = std::sqrt(distance_squared);
    float penetration_depth = radius_sum - distance;
    vec3 penetration_axis = (center2 - center1).normalized();
    transform1.position = transform1.position + penetration_axis * (penetration_depth * 0.5f);
    transform2.position = transform2.position - penetration_axis * (penetration_depth * 0.5f);
    return {true, penetration_depth, penetration_axis};
}
CollisionResult s_collision::intersects_obb_in_obb(const c_quad &obb1, const c_quad &obb2, c_transform &transform1,
                                                   c_transform &transform2) {
    // Extract rotation matrices
    const mat4 rotation_matrix1 = mat4::create_rotation_matrix(transform1.rotation);
    const mat4 rotation_matrix2 = mat4::create_rotation_matrix(transform2.rotation);

    const auto vertices1 = get_obb_points_in_world_space(obb1, transform1);
    const auto vertices2 = get_obb_points_in_world_space(obb2, transform2);

    // Extract axes
    vec3 axes1[3] = {
        rotation_matrix1.get_right_axis(),
        rotation_matrix1.get_up_axis(),
        rotation_matrix1.get_forward_axis()
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
        if (!test_axis(axis, vertices1, vertices2, min_penetration_depth,penetration_axis))
            return {false, 0, vec3()};
    }

    for (const auto &axis : axes2) {
        if (!test_axis(axis, vertices1, vertices2, min_penetration_depth,penetration_axis))
            return {false, 0, vec3()};
    }

    for (const auto &axis1 : axes1) {
        for (const auto &axis2 : axes2) {
            vec3 axis = axis1.cross_product(axis2);
            if (!test_axis(axis, vertices1, vertices2, min_penetration_depth,penetration_axis))
                return {false, 0, vec3()};
        }
    }

    transform1.position = transform1.position + penetration_axis * (min_penetration_depth * 0.5f);
    transform2.position = transform2.position - penetration_axis * (min_penetration_depth * 0.5f);
    return {true, min_penetration_depth, penetration_axis};
}
std::vector<vec3> s_collision::get_obb_points_in_world_space(const c_quad &obb, const c_transform &transform)
{
    // Extract rotation matrix from the transform
    mat4 rotation_matrix = mat4::create_rotation_matrix(transform.rotation);

    // Compute local OBB points
    std::vector<vec3> local_points = faces::get_quad_verticies(obb.extents);

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
    if (axis.length_squared() < 0.0001f) {
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
    }
    return true;
}
CollisionResult s_collision::intersects_obb_in_sphere(const c_quad &obb, c_sphere &sphere,
                                                      c_transform &obb_transform,
                                                      c_transform &sphere_transform) {
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
        return {false, 0, vec3()};
    }

    float distance = std::sqrt(distance_squared);
    float penetration_depth = radius_sphere - distance;
    vec3 penetration_axis = (rotation_matrix_obb * difference).normalized();

    // Adjust positions for penetration resolution
    obb_transform.position = obb_transform.position - penetration_axis * (penetration_depth * 0.5f);
    sphere_transform.position = sphere_transform.position + penetration_axis * (penetration_depth * 0.5f);

    return {true, penetration_depth, penetration_axis};
}
CollisionResult s_collision::intersects_obb_in_aabb(const c_quad &obb, const c_quad &aabb,
                                                    const c_transform &obb_transform, const c_transform &aabb_transform)
{
    return CollisionResult();
}
