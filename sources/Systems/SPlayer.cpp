#include "SPlayer.hpp"
#include "Components.hpp"
#include "Engine/EngineUtil.hpp"
#include "raylib.h"

#include <vector>

static constexpr float movespeed = 8.0f;
static constexpr float rotation_speed = 0.25f;

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
    bool space = engine_util::is_key_pressed(KEY_SPACE);

    for( const auto id : ids )
    {
        auto & acceleration = dynamic_bodies.get_item(id).acceleration;
        auto & angular_acceleration = dynamic_bodies.get_item(id).angular_acceleration;
        //velocity = { 0, 0, 0 };
        if( w ){
            acceleration.y += movespeed * dt;
            angular_acceleration.x += rotation_speed * dt;
        }
        if( a ){
            acceleration.x -= movespeed * dt;
            angular_acceleration.z -= rotation_speed * dt;
        }
        if( s ) {
            acceleration.y -= movespeed * dt;
            angular_acceleration.x -= rotation_speed * dt;
        }
        if( d ) {
            acceleration.x += movespeed * dt;
            angular_acceleration.z += rotation_speed * dt;
        }
        if( q ){
            acceleration.z += movespeed * dt;

        }
        if( e || space){
            acceleration.z -= movespeed * dt;
        }
    }
}

void s_player::shutdown()
{
}