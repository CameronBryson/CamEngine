#pragma once
#include "Components.hpp"
#include "Registry.hpp"

class s_collision
{
  public:
    static void update(const registry &registry);
    static bool intersects_aabb_in_aabb(const c_aabb &aabb1, const c_aabb &aabb2, const c_transform &transform1,
                                     const c_transform &transform2);
    static bool intersects_point_in_aabb(const vec3 &point, const c_aabb &aabb, const c_transform &aabb_transform);
    static bool intersects_sphere_in_aabb(const c_sphere &sphere, const c_aabb &aabb, const c_transform &sphere_transform,
                                       const c_transform &aabb_transform);
    static bool intersects_sphere_in_sphere(const c_sphere &sphere1, const c_sphere &sphere2, const c_transform &transform1,
                                         const c_transform &transform2);
};
