#include "SPlayer.hpp"

#include "Components.hpp"
#include "raylib.h"
static constexpr float MOVESPEED = 500.0f;
void SPlayer::Update(Registry& registry, float dt) {
    for (const auto ID: registry.getEntityIDS<CPlayer>()) {
        auto lock = registry.lockEntity(ID);
        auto& rb = registry.getComponent<CRigidBody>(ID);
        if (IsKeyDown(KEY_D)) rb.acceleration.x += MOVESPEED * dt;
        if (IsKeyDown(KEY_A)) rb.acceleration.x -= MOVESPEED * dt;
        if (IsKeyDown(KEY_W)) rb.acceleration.y -= MOVESPEED * dt;
        if (IsKeyDown(KEY_S)) rb.acceleration.y += MOVESPEED * dt;
    }
}

void SPlayer::Shutdown(){
}
SPlayer::SPlayer() {
    printf("Player system created\n");

}
SPlayer::~SPlayer() {
    printf("Player system destroyed\n");
}
