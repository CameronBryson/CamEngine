#pragma once
class Registry;
class SUI {
public:
    void init();
    void update(const Registry& registry);
    void shutdown();
};

