#include "SPlayer.hpp"
#include "Components.hpp"
#include "Engine/EngineUtil.hpp"
#include "raylib.h"

#include <vector>

static constexpr float movespeed = 8.0f;

void s_player::update(const registry & registry, const float dt)
{
    auto & dynamic_bodies = registry.get_sparse_set<c_dynamic_body>();
    auto ids = registry.get_entity_ids<c_player, c_dynamic_body>();

    bool w = engine_util::is_key_pressed('W');
    bool a = engine_util::is_key_pressed('A');
    bool s = engine_util::is_key_pressed('S');
    bool d = engine_util::is_key_pressed('D');
    bool q = engine_util::is_key_pressed('Q');
    bool e = engine_util::is_key_pressed('E');

    for( const auto id : ids )
    {
        auto & acceleration = dynamic_bodies.get_item(id).acceleration;
        //velocity = { 0, 0, 0 };
        if( w ) acceleration.z -= movespeed * dt;
        if( a ) acceleration.x -= movespeed * dt;
        if( s ) acceleration.z += movespeed * dt;
        if( d ) acceleration.x += movespeed * dt;
        if( q ) acceleration.y -= movespeed * dt;
        if( e ) acceleration.y += movespeed * dt;
    }
}

void s_player::shutdown()
{
}