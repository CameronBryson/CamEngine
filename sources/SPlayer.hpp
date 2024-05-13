#ifndef SPLAYER_HPP
#define SPLAYER_HPP
#include <Registry.hpp>

class SPlayer{
public:
  void Update(Registry& registry);
  void Shutdown();
private:
  void HandleInput();
};

#endif // SPLAYER_HPP
