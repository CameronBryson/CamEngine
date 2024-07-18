#ifndef CRENDER_HPP
#define CRENDER_HPP
#include "../glm/vec2.hpp"
struct CRender{
  explicit CRender(glm::vec2 renderOffset = {0,0}) : renderOffset(renderOffset) {}
  glm::vec2 renderOffset;
};
#endif // CRENDER_HPP
