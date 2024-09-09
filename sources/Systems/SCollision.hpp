#pragma once
#include "Components.hpp"
#include "Engine/Registry.hpp"

class s_collision
{
public:
    static void update(registry& registry);
    static bool point_in_aabb(glm::vec3 min, glm::vec3 max, glm::vec3 position, glm::vec3 point);
    static bool intersects_aabb_in_aabb(const glm::vec3& min1, const glm::vec3& max1, const glm::vec3& min2,
                                        const glm::vec3& max2);
    static bool intersects_sphere_in_sphere(unsigned id1, unsigned id2, registry& registry);

    static bool intersects_obb_in_obb(unsigned id1, unsigned id2, registry& registry);
    static bool intersects_obb_in_sphere(unsigned quad_id, unsigned sphere_id, registry& registry);
    static std::vector<glm::vec3> get_obb_points_in_world_space(const c_quad& obb, const c_transform& transform);
    static bool test_axis(const glm::vec3& axis, const std::vector<glm::vec3>& points1,
                          const std::vector<glm::vec3>& points2, float& overlap, glm::vec3& penetration_axis);
};
