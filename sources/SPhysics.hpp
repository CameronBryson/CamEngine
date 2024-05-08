#ifndef SPHYSICS_HPP
#define SPHYSICS_HPP

class SPhysics{
public:
  void Update();
  void Shutdown();
private:
  void UpdateKinematics();
};

#endif // SPHYSICS_HPP
