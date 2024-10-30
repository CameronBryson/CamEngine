#pragma once

#include "Components.hpp"

class collision_manifold
{
public:
  collision_manifold(const glm::vec3 normal, const float penetration_depth, c_transform& transform1, c_transform& transform2, c_dynamic_body* dynamic_body1,
      c_dynamic_body* dynamic_body2);
  ~collision_manifold() = default;

  void resolve_collision() const;

  float penetration_depth_ = 0;
  glm::vec3 normal;

private:
  void resolve_dynamic_vs_dynamic() const;

  void resolve_dynamic_vs_not_dynamic(bool is_first) const;

  c_dynamic_body* dynamic_body1;
  c_dynamic_body* dynamic_body2;
  c_transform& transform1_;
  c_transform& transform2_;
};
