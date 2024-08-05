#pragma once
#include <chrono>
#include <vector>

class stats
{
  public:
    enum stat_type
    {
        UPDATE,
        RENDER,
        BENCHMARK,
        DELTATIME,
        COUNT
    };
    static std::vector<std::chrono::duration<float, std::ratio<1, 100>>> timer_vector;
};
