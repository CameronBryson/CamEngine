#include "SPlayer.hpp"
#include "Components.hpp"
#include "raylib.h"
#include <vector>

static constexpr float movespeed = 500.0f;

void s_player::update(const registry &registry, const float dt)
{
    auto &rigidbodies = registry.get_sparse_set<c_rigid_body>();
    auto ids = registry.get_entity_ids<c_player, c_rigid_body>();

    const bool w = IsKeyDown(KEY_W);
    const bool a = IsKeyDown(KEY_A);
    const bool s = IsKeyDown(KEY_S);
    const bool d = IsKeyDown(KEY_D);
    const bool q = IsKeyDown(KEY_Q);
    const bool e = IsKeyDown(KEY_E);

    for (const auto id : ids)
    {
        auto &[mass, drag, acceleration, force] = rigidbodies.get_item(id);

        if (w)
            acceleration.y -= movespeed * dt;
        if (a)
            acceleration.x -= movespeed * dt;
        if (s)
            acceleration.y += movespeed * dt;
        if (d)
            acceleration.x += movespeed * dt;
        if (q)
            acceleration.z -= movespeed * dt;
        if (e)
            acceleration.z += movespeed * dt;
    }
}

void s_player::shutdown()
{
}
