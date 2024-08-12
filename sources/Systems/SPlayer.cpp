#include "SPlayer.hpp"
#include "Components.hpp"
#include "Engine/EngineUtil.hpp"
#include "raylib.h"

#include <vector>

static constexpr float movespeed = 1.0f;

void s_player::update(const registry & registry, const float dt)
{
    auto & velocities = registry.get_sparse_set<c_velocity>();
    auto ids = registry.get_entity_ids<c_player, c_velocity>();

    bool w = engine_util::is_key_pressed('W');
    bool a = engine_util::is_key_pressed('A');
    bool s = engine_util::is_key_pressed('S');
    bool d = engine_util::is_key_pressed('D');
    bool q = engine_util::is_key_pressed('Q');
    bool e = engine_util::is_key_pressed('E');

    for( const auto id : ids )
    {
        auto & [velocity] = velocities.get_item(id);
        velocity = { 0, 0, 0 };
        if( w )
            velocity.y += movespeed * dt;
        if( a )
            velocity.x -= movespeed * dt;
        if( s )
            velocity.y -= movespeed * dt;
        if( d )
            velocity.x += movespeed * dt;
        if( q )
            velocity.z -= movespeed * dt;
        if( e )
            velocity.z += movespeed * dt;
    }
}

void s_player::shutdown()
{
}