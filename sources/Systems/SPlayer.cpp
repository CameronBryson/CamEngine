#include "SPlayer.hpp"
#include "Components.hpp"
#include "raylib.h"
#include <vector>

static constexpr float movespeed = 1.0f;

void s_player::update(const registry &registry, const float dt)
{
    auto& velocities = registry.get_sparse_set<c_velocity>();
    auto ids = registry.get_entity_ids<c_player, c_velocity>();

    const bool w = IsKeyDown(KEY_W);
    const bool a = IsKeyDown(KEY_A);
    const bool s = IsKeyDown(KEY_S);
    const bool d = IsKeyDown(KEY_D);
    const bool q = IsKeyDown(KEY_Q);
    const bool e = IsKeyDown(KEY_E);

    for (const auto id : ids)
    {
        auto &[velocity] = velocities.get_item(id);
        velocity = {0,0,0};
        if (w)
            velocity.y -= movespeed * dt;
        if (a)
            velocity.x -= movespeed * dt;
        if (s)
            velocity.y += movespeed * dt;
        if (d)
            velocity.x += movespeed * dt;
        if (q)
            velocity.z -= movespeed * dt;
        if (e)
            velocity.z += movespeed * dt;
    }
}

void s_player::shutdown()
{
}
