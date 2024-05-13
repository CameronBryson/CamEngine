#ifndef SPHYSICS_HPP
#define SPHYSICS_HPP
#include <Registry.hpp>
class SPhysics{
public:
  void Update(Registry& registry);
  void Shutdown();
private:
  void UpdateKinematics();
};

#endif // SPHYSICS_HPP
