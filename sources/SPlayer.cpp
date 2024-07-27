#include "SPlayer.hpp"
#include <vector>
#include <mutex>
#include "Components.hpp"
#include "raylib.h"

static constexpr float MOVESPEED = 500.0f;

void SPlayer::Update(Registry& registry, float dt) {
    std::vector<unsigned short> IDS = registry.getEntityIDS<CPlayer, CRigidBody>();

    bool D = IsKeyDown(KEY_D);
    bool A = IsKeyDown(KEY_A);
    bool W = IsKeyDown(KEY_W);
    bool S = IsKeyDown(KEY_S);
    for (const auto& ID : IDS) {

        auto& rb = registry.getComponent<CRigidBody>(ID);
        if (D) rb.acceleration.x += MOVESPEED * dt;
        if (A) rb.acceleration.x -= MOVESPEED * dt;
        if (W) rb.acceleration.y -= MOVESPEED * dt;
        if (S) rb.acceleration.y += MOVESPEED * dt;
    }
}

void SPlayer::Shutdown() {
}

SPlayer::SPlayer() {
    printf("Player system created\n");
}

SPlayer::~SPlayer() {
    printf("Player system destroyed\n");
}