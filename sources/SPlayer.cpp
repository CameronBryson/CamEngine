#include "SPlayer.hpp"
#include <vector>
#include <mutex>
#include "Components.hpp"
#include "raylib.h"

static constexpr float MOVESPEED = 50.0f;

void SPlayer::Update(Registry& registry, float dt) {
    auto& rigidbodies = registry.getSparseSet<CRigidBody>();
    auto IDS = registry.getEntityIDS<CPlayer, CRigidBody>();

    bool D = IsKeyDown(KEY_D);
    bool A = IsKeyDown(KEY_A);
    bool W = IsKeyDown(KEY_W);
    bool S = IsKeyDown(KEY_S);

    for (auto ID : IDS) {
        auto& rb = rigidbodies.getItem(ID);
        if (D) rb.acceleration.x += MOVESPEED * dt;
        if (A) rb.acceleration.x -= MOVESPEED * dt;
        if (W) rb.acceleration.z -= MOVESPEED * dt;
        if (S) rb.acceleration.z += MOVESPEED * dt;
    }
}

void SPlayer::Shutdown() {
}
