#ifndef CBOXBOUNDS_HPP
#define CBOXBOUNDS_HPP

struct CBoxBounds{
  CBoxBounds(float width = 0 , float height = 0){
    this->width = width;
    this->height = height;
  }
  float width;
  float height;
};

#endif // CBOXBOUNDS_HPP
