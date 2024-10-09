#pragma once

class registry;
class s_player
{
public:
    static void update(registry& registry, float dt);
    static void shutdown();
};
