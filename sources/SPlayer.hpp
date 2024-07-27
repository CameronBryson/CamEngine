#pragma once
#include <Registry.hpp>

class SPlayer{
public:
    static void Update(Registry &registry, float dt);
    static void Shutdown();
};

