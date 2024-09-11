#include "SCollision.hpp"

#include "../Engine/Collision.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <glm/gtx/euler_angles.hpp>

void s_collision::update(registry & registry)
{
    // auto & obbs = registry.get_sparse_set<c_quad>();
    // auto & spheres = registry.get_sparse_set<c_sphere>();
    // auto & transforms = registry.get_sparse_set<c_transform>();
     //auto & colliders = registry.get_sparse_set<c_collider>();

    std::vector<unsigned short> obb_ids = registry.get_entity_ids<c_quad, c_transform, c_collider>();
    std::vector<unsigned short> sphere_ids = registry.get_entity_ids<c_sphere, c_transform, c_collider>();

    // Check OBB-OBB intersections
    for( std::vector<unsigned short>::size_type i = 0; i < obb_ids.size(); ++i )
    {
        for( std::vector<unsigned short>::size_type j = i + 1; j < obb_ids.size(); ++j )
        {
            intersects_obb_in_obb(obb_ids[i], obb_ids[j], registry);
        }
    }

    // Check OBB-SPHERE intersections
    for( auto id : obb_ids )
    {
        for( auto id2 : sphere_ids )
        {
            if( id == id2 ) continue;
            intersects_obb_in_sphere(id, id2, registry);
        }
    }
    //sphere-sphere
    for( auto id : sphere_ids )
    {
        for( auto id2 : sphere_ids )
        {
            intersects_sphere_in_sphere(id,id2, registry);
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

bool s_collision::intersects_aabb_in_aabb(const glm::vec3 & min1, const glm::vec3 & max1, const glm::vec3 & min2, const glm::vec3 & max2)
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

bool s_collision::intersects_sphere_in_sphere(unsigned id1, unsigned id2, registry& registry)
{
    auto& sphere1 = registry.get_component<c_sphere>(id1);
    auto& sphere2 = registry.get_component<c_sphere>(id2);
    auto& transform1 = registry.get_component<c_transform>(id1);
    auto& transform2 = registry.get_component<c_transform>(id2);
    auto& collider1 = registry.get_component<c_collider>(id1);
    auto& collider2 = registry.get_component<c_collider>(id2);

    if ((collider1.collision_type == object_collision_type::STATIC && collider2.collision_type == object_collision_type::STATIC) ||
        (collider1.collision_bitmask & collider2.collision_bitmask) == 0)
    {
        EventHandler::GetInstance()->collision_dispatcher.SendEvent(CollisionNotDetectedEvent(id1, id2));
        return false;
    }

    const float distance_squared = glm::length2(transform1.position - transform2.position);
    const float radius_sum = sphere1.radius + sphere2.radius;

    if (distance_squared > (radius_sum * radius_sum))
    {
        EventHandler::GetInstance()->collision_dispatcher.SendEvent(CollisionNotDetectedEvent(id1, id2));
        return false;
    }

    float penetration_depth = radius_sum - std::sqrt(distance_squared);
    glm::vec3 penetration_axis = glm::normalize(transform2.position - transform1.position);

    if (penetration_depth >= 0)
    {
        collision_manifold manifold{penetration_axis, penetration_depth, transform1, transform2, collider1.collision_type, collider2.collision_type};
        EventHandler::GetInstance()->collision_dispatcher.SendEvent(CollisionDetectedEvent(manifold, id1, id2));
    }

    return true;
}

bool s_collision::intersects_obb_in_obb(const unsigned id1, const unsigned id2, registry & registry)
{

    auto & obb1 = registry.get_component<c_quad>(id1);
    auto & obb2 = registry.get_component<c_quad>(id2);
    auto & transform1 = registry.get_component<c_transform>(id1);
    auto & transform2 = registry.get_component<c_transform>(id2);
    auto & collider1 = registry.get_component<c_collider>(id1);
    auto & collider2 = registry.get_component<c_collider>(id2);


    const object_collision_type type1 = collider1.collision_type;
    const object_collision_type type2 = collider2.collision_type;
    if( type1 == object_collision_type::STATIC && type2 == object_collision_type::STATIC )
    {
        EventHandler::GetInstance()->collision_dispatcher.SendEvent(CollisionNotDetectedEvent(id1, id2));
        return false;
    }
    const auto object_bitmask = collider1.collision_bitmask;
    const auto other_object_bitmask = collider2.collision_bitmask;
    if( (object_bitmask & other_object_bitmask) == 0 )
    {
        EventHandler::GetInstance()->collision_dispatcher.SendEvent(CollisionNotDetectedEvent(id1, id2));
        return false;
    }
    // Extract rotation matrices
    const glm::mat4 rotation_matrix1 = glm::eulerAngleXYZ(transform1.rotation.x, transform1.rotation.y, transform1.rotation.z);
    const glm::mat4 rotation_matrix2 = glm::eulerAngleXYZ(transform2.rotation.x, transform2.rotation.y, transform2.rotation.z);

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
    for( const auto & axis : axes1 )
    {
        if( ! test_axis(axis, vertices1, vertices2, min_penetration_depth, penetration_axis) ||
            ! test_axis(axis * -1.0f, vertices1, vertices2, min_penetration_depth, penetration_axis) )
        {
            EventHandler::GetInstance()->collision_dispatcher.SendEvent(CollisionNotDetectedEvent(id1, id2));
            return false;
        }
    }

    for( const auto & axis : axes2 )
    {
        if( ! test_axis(axis, vertices1, vertices2, min_penetration_depth, penetration_axis) ||
            ! test_axis(axis * -1.0f, vertices1, vertices2, min_penetration_depth, penetration_axis) )
        {
            EventHandler::GetInstance()->collision_dispatcher.SendEvent(CollisionNotDetectedEvent(id1, id2));
            return false;
        }
    }

    for( const auto & axis1 : axes1 )
    {
        for( const auto & axis2 : axes2 )
        {
            glm::vec3 axis = cross(axis1, axis2);
            if( ! test_axis(axis, vertices1, vertices2, min_penetration_depth, penetration_axis) ||
                ! test_axis(axis * -1.0f, vertices1, vertices2, min_penetration_depth, penetration_axis) )
            {
                EventHandler::GetInstance()->collision_dispatcher.SendEvent(CollisionNotDetectedEvent(id1,id2));
                return false;
            }
        }
    }

    if( min_penetration_depth >= 0 )
    {
        collision_manifold manifold{penetration_axis, min_penetration_depth, transform1, transform2, type1, type2};
        EventHandler::GetInstance()->collision_dispatcher.SendEvent(CollisionDetectedEvent(manifold, id1, id2));
    }

    return true;
}

std::vector<glm::vec3> s_collision::get_obb_points_in_world_space(const c_quad & obb, const c_transform & transform)
{
    // Extract rotation matrix from the transform
    const glm::mat4 rotation_matrix = glm::eulerAngleXYZ(transform.rotation.x, transform.rotation.y, transform.rotation.z);

    // Compute local OBB points from obb.extents
    std::vector<glm::vec3> local_points = {
        glm::vec3(-obb.extents.x, -obb.extents.y, -obb.extents.z),
        glm::vec3(obb.extents.x, -obb.extents.y, -obb.extents.z),
        glm::vec3(obb.extents.x, obb.extents.y, -obb.extents.z),
        glm::vec3(-obb.extents.x, obb.extents.y, -obb.extents.z),
        glm::vec3(-obb.extents.x, -obb.extents.y, obb.extents.z),
        glm::vec3(obb.extents.x, -obb.extents.y, obb.extents.z),
        glm::vec3(obb.extents.x, obb.extents.y, obb.extents.z),
        glm::vec3(-obb.extents.x, obb.extents.y, obb.extents.z)
    };

    // Transform local points to world space
    std::vector<glm::vec3> world_points;
    world_points.reserve(local_points.size());
    for( const auto & point : local_points )
    {
        glm::vec3 scaled_point = point * transform.scale;
        glm::vec3 rotated_point = rotation_matrix * glm::vec4(scaled_point, 1);
        glm::vec3 world_point = rotated_point + transform.position;
        world_points.push_back(world_point);
    }

    return world_points;
}

bool s_collision::test_axis(const glm::vec3 & axis, const std::vector<glm::vec3> & points1, const std::vector<glm::vec3> & points2, float & overlap,
    glm::vec3 & penetration_axis)
{
    if( length2(axis) < 0.00001f )
    {
        return true;
    }

    float min1 = std::numeric_limits<float>::max();
    float max1 = std::numeric_limits<float>::lowest();
    float min2 = std::numeric_limits<float>::max();
    float max2 = std::numeric_limits<float>::lowest();

    for( const auto & point : points1 )
    {
        float projection = dot(point, axis);
        if( projection < min1 ) min1 = projection;
        if( projection > max1 ) max1 = projection;
    }

    for( const auto & point : points2 )
    {
        float projection = dot(point, axis);
        if( projection < min2 ) min2 = projection;
        if( projection > max2 ) max2 = projection;
    }

    float axis_overlap = std::min(max1, max2) - std::max(min1, min2);
    if( axis_overlap < 0 )
    {
        return false;
    }

    if( axis_overlap < overlap )
    {
        overlap = axis_overlap;
        penetration_axis = axis;
        if( min2 > min1 )
        {
            penetration_axis = axis * -1.0f;
        }
    }
    return true;
}

bool s_collision::intersects_obb_in_sphere(unsigned quad_id, unsigned sphere_id, registry& registry)
{
    auto& obb = registry.get_component<c_quad>(quad_id);
    auto& sphere = registry.get_component<c_sphere>(sphere_id);
    auto& obb_transform = registry.get_component<c_transform>(quad_id);
    auto& sphere_transform = registry.get_component<c_transform>(sphere_id);
    auto& obb_collider = registry.get_component<c_collider>(quad_id);
    auto& sphere_collider = registry.get_component<c_collider>(sphere_id);

    if ((obb_collider.collision_type == object_collision_type::STATIC && sphere_collider.collision_type == object_collision_type::STATIC) ||
        (obb_collider.collision_bitmask & sphere_collider.collision_bitmask) == 0)
    {
        EventHandler::GetInstance()->collision_dispatcher.SendEvent(CollisionNotDetectedEvent(quad_id, sphere_id));
        return false;
    }

    const glm::vec3 extents_obb = obb.extents * 0.5f;
    const glm::mat4 rotation_matrix_obb = glm::eulerAngleXYZ(obb_transform.rotation.x, obb_transform.rotation.y, obb_transform.rotation.z);

    glm::vec3 local_center_sphere = glm::transpose(rotation_matrix_obb) * glm::vec4(sphere_transform.position - obb_transform.position, 1.0f);

    glm::vec3 closest_point = glm::clamp(local_center_sphere, -extents_obb, extents_obb);

    glm::vec3 difference = local_center_sphere - closest_point;
    float penetration_depth = sphere.radius - glm::length(difference);
    if (penetration_depth < 0)
    {
        EventHandler::GetInstance()->collision_dispatcher.SendEvent(CollisionNotDetectedEvent(quad_id, sphere_id));
        return false;
    }

    glm::vec3 penetration_axis = glm::normalize(rotation_matrix_obb * glm::vec4(difference, 1.0f));
    collision_manifold manifold{penetration_axis, penetration_depth, obb_transform, sphere_transform, obb_collider.collision_type, sphere_collider.collision_type};
    EventHandler::GetInstance()->collision_dispatcher.SendEvent(CollisionDetectedEvent(manifold, quad_id, sphere_id));
    return true;
}
