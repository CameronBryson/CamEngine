#pragma once
#include "Delegate/Delegate.hpp"
#include <vector>
class Signal{
public:
  void Connect();
  void Emit();
private:
  std::vector<Delegate> connections;
};