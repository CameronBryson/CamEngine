#pragma once
#include "Stats.hpp"
#include <chrono>

class timer
{
public:
    explicit timer(const stats::stat_type type)
    {
        start_ = std::chrono::system_clock::now();
        stat_type_ = type;
    };

    ~timer()
    {
        end_ = std::chrono::system_clock::now();
        const std::chrono::duration<float, std::ratio<1, 1000>> final = end_ - start_;
        // Assuming statType is an enum and can be cast to size_t
        stats::timer_vector[stat_type_] = final;
    }

private:
    std::chrono::time_point<std::chrono::system_clock> start_, end_;
    stats::stat_type stat_type_;
};
