#pragma once
#include "Stats.hpp"
#include <chrono>

class timer
{
public:
  explicit timer(const stats::stat_type type);

  ~timer();
private:
    std::chrono::time_point<std::chrono::system_clock> start_, end_;
    stats::stat_type stat_type_;
};
