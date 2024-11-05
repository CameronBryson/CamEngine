#pragma once

#include "Components.hpp"

class CollisionManifold
{
public:
  CollisionManifold(glm::vec3 normal, float penetration_depth, CTransform& transform1, CTransform& transform2, CDynamicBody* dynamic_body1,
      CDynamicBody* dynamic_body2);
  ~CollisionManifold() = default;

  void resolveCollision() const;

  float penetrationDepth = 0;
  glm::vec3 normal;

private:
  void resolveDynamicVsDynamic() const;

  void resolveDynamicVsNotDynamic(bool is_first) const;

  CDynamicBody* mDynamicBody1;
  CDynamicBody* mDynamicBody2;
  CTransform& mTransform1;
  CTransform& mTransform2;
};
