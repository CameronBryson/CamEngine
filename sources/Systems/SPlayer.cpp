#include "SPlayer.hpp"
#include "Components.hpp"
#include "Engine/EngineUtil.hpp"
#include "raylib.h"

#include <vector>

static constexpr float forward_movespeed = 1.0f;
static constexpr float movespeed = 10.0f;
static constexpr float rotation_speed = 0.25f;

void s_player::update(const registry & registry, const float dt)
{
    auto & dynamic_bodies = registry.get_sparse_set<c_dynamic_body>();
    auto & players = registry.get_sparse_set<c_player>();
    auto & transforms = registry.get_sparse_set<c_transform>();
    auto ids = registry.get_entity_ids<c_player, c_dynamic_body, c_transform>();

    bool w = engine_util::is_key_pressed('W');
    bool a = engine_util::is_key_pressed('A');
    bool s = engine_util::is_key_pressed('S');
    bool d = engine_util::is_key_pressed('D');
    bool q = engine_util::is_key_pressed('Q');
    bool e = engine_util::is_key_pressed('E');
    bool space = engine_util::is_key_pressed(KEY_SPACE);

    for( const auto id : ids )
    {
        //maybe add some kind of cap so that the ship doesnt rotate beyond 90 degrees
        auto & player = players.get_item(id);
        auto & dynamic_body = dynamic_bodies.get_item(id);
        //velocity = { 0, 0, 0 };
        if( w ){
            dynamic_body.acceleration.y += movespeed * dt;
            dynamic_body.angular_acceleration.x += rotation_speed * dt;
        }
        if( a ){
            dynamic_body.acceleration.x -= movespeed * dt;
            dynamic_body.angular_acceleration.z -= rotation_speed * dt;
        }
        if( s ) {
            dynamic_body.acceleration.y -= movespeed * dt;
            dynamic_body.angular_acceleration.x -= rotation_speed * dt;
        }
        if( d ) {
            dynamic_body.acceleration.x += movespeed * dt;
            dynamic_body.angular_acceleration.z += rotation_speed * dt;
        }
        if( q ){
            dynamic_body.acceleration.z += forward_movespeed * dt;

        }
        if( e || space){
            dynamic_body.acceleration.z -= forward_movespeed * dt;
        }
    }

}

void s_player::shutdown()
{
}