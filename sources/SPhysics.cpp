#include "SPhysics.hpp"

#include <cmath>
#include <execution>
#include <pstl/glue_execution_defs.h>

#include "Components.hpp"
#include "raylib.h"

void SPhysics::Update(Registry& registry, float dt) {
    // Iterate over entities that have CRigidBody, CTransform, and CVelocity components
    std::vector<unsigned short> IDS = registry.getEntityIDS<CRigidBody, CPosition, CVelocity>();
    std::for_each(std::execution::par_unseq,
        std::begin(IDS), std::end(IDS), [&registry, dt](const unsigned short ID) {
        auto lock = registry.lockEntity(ID);
        auto& rb = registry.getComponent<CRigidBody>(ID);
        auto& position = registry.getComponent<CPosition>(ID);
        auto& velocity = registry.getComponent<CVelocity>(ID);

        // Apply acceleration to velocity
        velocity.x += rb.acceleration.x * dt;
        velocity.y += rb.acceleration.y * dt;

        // Apply drag to velocity
        velocity.x *= std::pow(1 - rb.drag, dt);
        velocity.y *= std::pow(1 - rb.drag, dt);

        // Update position based on velocity
        position.x += velocity.x;
        position.y += velocity.y;

        // Reset acceleration
        rb.acceleration = {0, 0};
    });
}

void SPhysics::Shutdown() {
}

SPhysics::SPhysics() {
    printf("Physics system created\n");
}

SPhysics::~SPhysics() {
    printf("Physics system destroyed\n");
}