#pragma once
#include <Registry.hpp>
class SPhysics{
public:
    static void Update(Registry &registry, float dt);
    static void Shutdown();
};
