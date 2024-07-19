#include "SPhysics.hpp"

#include <cmath>

#include "CRigidBody.hpp"
#include "CTransform.hpp"
#include "raylib.h"


void SPhysics::Update(Registry* registry, float dt) {
    for (const auto ID: registry->getSparseSet<CRigidBody>()->getIDS()) {
        auto lock = registry->lockEntity(ID);
        auto& rb = registry->getComponent<CRigidBody>(ID);
        auto& transform = registry->getComponent<CTransform>(ID);
        rb.velocity += rb.acceleration * dt;
        rb.velocity *= std::pow(1 - rb.drag, dt);
        transform.position += rb.velocity;
        rb.acceleration = {0, 0};
    }
}
void SPhysics::Shutdown(){
  
}
SPhysics::SPhysics() {
    printf("Physics system created\n");
}
SPhysics::~SPhysics() {
    printf("Physics system destroyed\n");
}
