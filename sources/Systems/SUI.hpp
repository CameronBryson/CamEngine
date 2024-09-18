#pragma once
#include "Engine/Registry.hpp"
class s_ui {
public:
    static void init();
    static void update(const registry& registry);
    static void shutdown();
};

