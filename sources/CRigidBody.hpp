#ifndef CRIGIDBODY_HPP
#define CRIGIDBODY_HPP

#include "../glm/ext/vector_float2.hpp"
struct CRigidBody{
  explicit CRigidBody(float mass = 0, float drag = 0, float angularDrag = 0) :
        mass(mass), drag(drag), angularDrag(angularDrag), velocity(), acceleration(), angularVelocity(0) {}

  float mass;
  float drag;
  float angularDrag;
  glm::vec2 velocity;
  glm::vec2 acceleration;
  float angularVelocity;
};

#endif // CRIGIDBODY_HPP
