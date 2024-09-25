#pragma once
#include "Engine/Registry.hpp"

class s_player
{
public:
    static void init();
    static void update(registry& registry, float dt);
    static void shutdown();
    static void on_input_start_event(const Event<InputEvents>& event);
    static void on_input_hold_event(const Event<InputEvents>& event);
    static void on_input_end_event(const Event<InputEvents>& event);
};
