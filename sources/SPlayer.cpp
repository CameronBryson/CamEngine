#include "SPlayer.hpp"
#include "Components.hpp"
#include "raylib.h"
#include <vector>

static constexpr float movespeed = 5.0f;

void s_player::update(const registry &registry, const float dt)
{
    auto &rigidbodies = registry.get_sparse_set<c_rigid_body>();
    auto ids = registry.get_entity_ids<c_player, c_rigid_body>();

    const bool d = IsKeyDown(KEY_D);
    const bool a = IsKeyDown(KEY_A);
    const bool w = IsKeyDown(KEY_W);
    const bool s = IsKeyDown(KEY_S);

    for (const auto id : ids)
    {
        auto &[mass, drag, acceleration, force] = rigidbodies.get_item(id);
        if (d)
            acceleration.x += movespeed * dt;
        if (a)
            acceleration.x -= movespeed * dt;
        if (w)
            acceleration.z -= movespeed * dt;
        if (s)
            acceleration.z += movespeed * dt;
    }
}

void s_player::shutdown()
{
}
