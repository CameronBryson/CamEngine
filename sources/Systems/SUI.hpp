#pragma once
class registry;
class s_ui {
public:
    void init();
    void update(const registry& registry);
    void shutdown();
};

