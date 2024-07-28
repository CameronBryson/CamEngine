#pragma once
#include <Registry.hpp>

class SRender{
public:
    static void Init();
    static void Update(Registry &registry);
    static void Shutdown();
private:
    static void DrawStatistics();
};

