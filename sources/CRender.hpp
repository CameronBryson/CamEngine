#ifndef CRENDER_HPP
#define CRENDER_HPP

#include <glm/vec2.hpp>
struct CRender{
  CRender(glm::vec2 renderOffset = {0,0}){
    this->renderOffset = renderOffset;
  }
  glm::vec2 renderOffset;
};

#endif // CRENDER_HPP
