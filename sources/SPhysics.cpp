#include "SPhysics.hpp"
#include <cmath>
#include <vector>
#include <algorithm>
#include <mutex>
#include "Components.hpp"
#include "raylib.h"

void SPhysics::Update(Registry& registry, float dt) {
    auto& rigidbodies = registry.getSparseSet<CRigidBody>();
    auto& positions = registry.getSparseSet<CTransform>();
    auto& velocities = registry.getSparseSet<CVelocity>();
    auto IDS = registry.getEntityIDS<CRigidBody, CTransform, CVelocity>();

    for (auto ID : IDS) {
        auto& rb = rigidbodies.getItem(ID);
        auto& position = positions.getItem(ID);
        auto& velocity = velocities.getItem(ID);

        velocity.velocity += rb.acceleration * dt;

        velocity.velocity *= std::pow(1 - rb.drag, dt);

        position.position += velocity.velocity;

        rb.acceleration = {0, 0, 0};
    }
}

void SPhysics::Shutdown() {
}
