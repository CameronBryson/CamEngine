#ifndef SRENDER_HPP
#define SRENDER_HPP
#include <Registry.hpp>

class SRender{
public:
  void Init();
  void Update(Registry& registry);
  void Shutdown();
private:
};

#endif // SRENDER_HPP
