#include "SPlayer.hpp"
#include <execution>
#include "Components.hpp"
#include "raylib.h"
static constexpr float MOVESPEED = 500.0f;
void SPlayer::Update(Registry& registry, float dt) {
    std::vector<unsigned short> IDS = registry.getEntityIDS<CPlayer, CRigidBody>();
    std::for_each(std::execution::par_unseq,
        std::begin(IDS), std::end(IDS), [&registry, dt](const unsigned short ID) {
        auto& rb = registry.getComponent<CRigidBody>(ID);
        if (IsKeyDown(KEY_D)) rb.acceleration.x += MOVESPEED * dt;
        if (IsKeyDown(KEY_A)) rb.acceleration.x -= MOVESPEED * dt;
        if (IsKeyDown(KEY_W)) rb.acceleration.y -= MOVESPEED * dt;
        if (IsKeyDown(KEY_S)) rb.acceleration.y += MOVESPEED * dt;
    });
}

void SPlayer::Shutdown(){
}
SPlayer::SPlayer() {
    printf("Player system created\n");

}
SPlayer::~SPlayer() {
    printf("Player system destroyed\n");
}
