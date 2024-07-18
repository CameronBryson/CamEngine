#ifndef CTRANSFORM_HPP
#define CTRANSFORM_HPP

#include "../glm/ext/vector_float2.hpp"
struct CTransform{
  explicit CTransform(glm::vec2 position = {0,0}, float rotation = 0) : position(position), rotation(rotation) {}
  glm::vec2 position;
  float rotation;
};

#endif // CTRANSFORM_HPP
