#ifndef SPHYSICS_HPP
#define SPHYSICS_HPP
#include <Registry.hpp>
class SPhysics{
public:
    SPhysics();
    ~SPhysics();
    static void Update(Registry &registry, float dt);
    static void Shutdown();
private:
  void UpdateKinematics();
};

#endif // SPHYSICS_HPP
