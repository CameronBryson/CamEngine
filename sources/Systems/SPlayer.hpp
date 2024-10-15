#pragma once

class registry;
class s_player
{
public:
    void update(registry& registry, float dt);
    void shutdown();
};
