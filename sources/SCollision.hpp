#pragma once
#include "Components.hpp"
#include "Registry.hpp"
struct CollisionResult {
  bool intersects;
  float penetration_depth;
  vec3 penetration_axis;
};
class s_collision
{
  public:
    static void update(const registry &registry);
    static CollisionResult intersects_aabb_in_aabb(const c_quad &aabb1, const c_quad &aabb2, c_transform &transform1,
                                     c_transform &transform2);
    static CollisionResult intersects_sphere_in_aabb(const c_sphere &sphere, const c_quad &aabb, c_transform &sphere_transform,
                                       c_transform &aabb_transform);
    static CollisionResult intersects_sphere_in_sphere(const c_sphere &sphere1, const c_sphere &sphere2, c_transform &transform1,
                                         c_transform &transform2);

    static CollisionResult intersects_obb_in_obb(const c_quad &obb1, const c_quad &obb2,c_transform &transform1,
                                   c_transform &transform2);
    static CollisionResult intersects_obb_in_aabb(const c_quad &obb, const c_quad &aabb, const c_transform &obb_transform,
                                      const c_transform &aabb_transform);
    static CollisionResult intersects_obb_in_sphere(const c_quad &obb, c_sphere &sphere,
                                        c_transform &obb_transform, c_transform &sphere_transform);
    static std::vector<vec3> get_obb_points_in_world_space(const c_quad &obb, const c_transform &transform);
    static bool test_axis (const vec3 &axis, const std::vector<vec3> &points1, const std::vector<vec3> &points2, float &overlap, vec3 &penetration_axis);
};
