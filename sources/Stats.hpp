#pragma once
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

