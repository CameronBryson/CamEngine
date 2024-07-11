#ifndef SRENDER_HPP
#define SRENDER_HPP
#include <Registry.hpp>

class SRender{
public:
  void Init();
  void Update(Registry* registry);
  void Shutdown();
private:
  void DrawStatistics();
  double time = 0;
};

#endif // SRENDER_HPP
