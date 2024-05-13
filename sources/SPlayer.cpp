#include "SPlayer.hpp"

#include "CPlayer.hpp"
#include "CRigidbody.hpp"
#include "CTransform.hpp"
#include "raylib.h"
static constexpr float MOVESPEED = 5.0f;
void SPlayer::Update(Registry& registry) {
    for (auto ID: registry.getSparseSet<CPlayer>()->getIDS()) {
        auto& rb = registry.getComponent<CRigidbody>(ID);
        if (IsKeyDown(KEY_D)) rb.acceleration.x += MOVESPEED;
        if (IsKeyDown(KEY_A)) rb.acceleration.x-= MOVESPEED;
        if (IsKeyDown(KEY_W)) rb.acceleration.y -= MOVESPEED;
        if (IsKeyDown(KEY_S)) rb.acceleration.y += MOVESPEED;
    }
}

void SPlayer::Shutdown(){
  
}