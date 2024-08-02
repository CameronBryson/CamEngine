#pragma once
#include "Components.hpp"
#include "Registry.hpp"
class s_collision
{
  public:
    static void update(registry &registry);
    static bool intersects_sphere_in_sphere(const c_sphere &sphere1, const c_sphere &sphere2, c_transform &transform1,
                                         c_transform &transform2,registry &registry);

    static bool intersects_obb_in_obb(const c_quad &obb1, const c_quad &obb2,c_transform &transform1,
                                   c_transform &transform2,registry &registry);
    static bool intersects_obb_in_sphere(const c_quad &obb, c_sphere &sphere,
                                        c_transform &obb_transform, c_transform &sphere_transform,registry &registry);
    static std::vector<vec3> get_obb_points_in_world_space(const c_quad &obb, const c_transform &transform);
    static bool test_axis (const vec3 &axis, const std::vector<vec3> &points1, const std::vector<vec3> &points2, float &overlap, vec3 &penetration_axis);
};
