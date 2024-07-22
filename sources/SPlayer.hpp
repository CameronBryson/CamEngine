#ifndef SPLAYER_HPP
#define SPLAYER_HPP
#include <Registry.hpp>

class SPlayer{
public:
    SPlayer();
    ~SPlayer();
    static void Update(Registry &registry, float dt);
    static void Shutdown();
private:
  void HandleInput();
};

#endif // SPLAYER_HPP
