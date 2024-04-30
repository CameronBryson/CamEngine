#ifndef SPLAYER_HPP
#define SPLAYER_HPP

#include "ISystem.hpp"
class SPlayer final : public ISystem{
public:
  void Update() override;
  void Shutdown() override;
private:
  void HandleInput();
};

#endif // SPLAYER_HPP
