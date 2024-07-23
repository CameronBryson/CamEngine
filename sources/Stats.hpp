//
// Created by cam on 16/07/24.
//

#ifndef STATS_HPP
#define STATS_HPP
#include <chrono>
#include <vector>


class Stats {
public:
    enum StatType{
        UPDATE,
        RENDER,
        BENCHMARK,
        COUNT
    };
    static std::vector<std::chrono::duration<float,std::ratio<1,100>>> timerVector;
};



#endif //STATS_HPP
