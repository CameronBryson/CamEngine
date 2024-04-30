#ifndef SPHYSICS_HPP
#define SPHYSICS_HPP

#include "ISystem.hpp"
class SPhysics final : public ISystem{
public:
  void Update() override;
  void Shutdown() override;
private:
  void UpdateKinematics();
};

#endif // SPHYSICS_HPP
