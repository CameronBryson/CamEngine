#include "SPhysics.hpp"
#include <cmath>
#include <vector>
#include <algorithm>
#include <mutex>
#include "Components.hpp"
#include "raylib.h"

void SPhysics::Update(Registry& registry, float dt) {
    auto& rigidbodies = registry.getSparseSet<CRigidBody>();
    auto& positions = registry.getSparseSet<CPosition>();
    auto& velocities = registry.getSparseSet<CVelocity>();
    auto IDS = registry.getEntityIDS<CRigidBody, CPosition, CVelocity>();

    for (auto ID : IDS) {
        auto& rb = rigidbodies.getItem(ID);
        auto& position = positions.getItem(ID);
        auto& velocity = velocities.getItem(ID);

        velocity.x += rb.acceleration.x * dt;
        velocity.y += rb.acceleration.y * dt;

        velocity.x *= std::pow(1 - rb.drag, dt);
        velocity.y *= std::pow(1 - rb.drag, dt);

        position.x += velocity.x;
        position.y += velocity.y;

        rb.acceleration = {0, 0};
    }
}

void SPhysics::Shutdown() {
}
