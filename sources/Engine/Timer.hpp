#pragma once
#include "Stats.hpp"
#include <chrono>

class Timer
{
public:
  explicit Timer(const Stats::stat_type type);

  ~Timer();
private:
    std::chrono::time_point<std::chrono::system_clock> start_, end_;
    Stats::stat_type stat_type_;
};
