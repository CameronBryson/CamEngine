#ifndef SRENDER_HPP
#define SRENDER_HPP

#include "ISystem.hpp"
class SRender final :public ISystem{
public:
  void Update() override;
  void Shutdown() override;
private:
};

#endif // SRENDER_HPP
