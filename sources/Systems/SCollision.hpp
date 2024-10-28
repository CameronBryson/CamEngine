#pragma once
#include "Engine/SparseSet.hpp"
#include "glm/glm.hpp"
class registry;
class c_sphere;
class c_quad;
class c_transform;
class c_collider;
class c_dynamic_body;

class s_collision
{
public:
    void update(registry& registry);
private:
    bool point_in_aabb(glm::vec3 min, glm::vec3 max, glm::vec3 position, glm::vec3 point);
    bool intersects_aabb_in_aabb(const glm::vec3& min1, const glm::vec3& max1, const glm::vec3& min2,
                                        const glm::vec3& max2);
    bool intersects_sphere_in_sphere(unsigned id1, unsigned id2, sparse_set<c_sphere>& spheres, sparse_set<c_transform>& transforms, sparse_set<c_collider>& colliders, sparse_set<c_dynamic_body>& dynamic_bodies);
    bool intersects_obb_in_obb(unsigned id1, unsigned id2, sparse_set<c_quad>& quads, sparse_set<c_transform>& transforms, sparse_set<c_collider>& colliders, sparse_set<c_dynamic_body>& dynamic_bodies);
    bool intersects_obb_in_sphere(unsigned quad_id, unsigned sphere_id, sparse_set<c_quad>& quads, sparse_set<c_sphere>& spheres, sparse_set<c_transform>& transforms, sparse_set<c_collider>& colliders, sparse_set<c_dynamic_body>& dynamic_bodies);
    std::vector<glm::vec3> get_obb_points_in_world_space(const c_quad& obb, const c_transform& transform);
    bool test_axis(const glm::vec3& axis, const std::vector<glm::vec3>& points1,
                          const std::vector<glm::vec3>& points2, float& overlap, glm::vec3& penetration_axis);
};
