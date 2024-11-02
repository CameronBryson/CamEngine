#include "Timer.hpp" 
Timer::Timer(const Stats::stat_type type)
{
    start_ = std::chrono::system_clock::now();
    stat_type_ = type;
};
Timer::~Timer()
{
    end_ = std::chrono::system_clock::now();
    const std::chrono::duration<float, std::ratio<1, 1000>> final = end_ - start_;
    // Assuming statType is an enum and can be cast to size_t
    Stats::timer_vector[stat_type_] = final;
}
