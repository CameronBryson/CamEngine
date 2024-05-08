#ifndef SPLAYER_HPP
#define SPLAYER_HPP

class SPlayer{
public:
  void Update();
  void Shutdown();
private:
  void HandleInput();
};

#endif // SPLAYER_HPP
