#pragma once
class registry;
class s_ui {
public:
    static void init();
    static void update(const registry& registry);
    static void shutdown();
};

