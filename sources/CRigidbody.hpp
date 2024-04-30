#ifndef CRIGIDBODY_HPP
#define CRIGIDBODY_HPP

#include "glm/ext/vector_float2.hpp"
struct CRigidbody{
  CRigidbody(float mass = 0, float drag = 0, float angularDrag = 0){
    this->mass = mass;
    this->drag = drag;
    this->angularDrag = angularDrag;
    this->velocity = {0,0};
    this->acceleration = {0,0};
    this->angularVelocity = 0;
  }
  float mass;
  float drag;
  float angularDrag;
  glm::vec2 velocity;
  glm::vec2 acceleration;
  float angularVelocity;
};

#endif // CRIGIDBODY_HPP
