#ifndef TIMER_HPP
#define TIMER_HPP
#include <chrono>
#include "Stats.hpp"

class Timer {
public:
    explicit Timer(Stats::StatType type) {
        start = std::chrono::system_clock::now();
        statType = type;
    };
    ~Timer() {
        end = std::chrono::system_clock::now();
        std::chrono::duration<float, std::ratio<1,1000>> final = end - start;
        // Assuming statType is an enum and can be cast to size_t
        Stats::timerVector[statType] = final;
    }
private:
    std::chrono::time_point<std::chrono::system_clock> start, end;
    Stats::StatType statType;
};
#endif //TIMER_HPP