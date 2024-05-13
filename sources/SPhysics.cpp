#include "SPhysics.hpp"

#include <cmath>

#include "CRigidbody.hpp"
#include "CTransform.hpp"
#include "raylib.h"


void SPhysics::Update(Registry& registry) {
    for (auto ID: registry.getSparseSet<CRigidbody>()->getIDS()) {
        auto& rb = registry.getComponent<CRigidbody>(ID);
        auto& transform = registry.getComponent<CTransform>(ID);
        rb.velocity += rb.acceleration * GetFrameTime();
        rb.velocity *= std::pow(1 - rb.drag, GetFrameTime());
        transform.position += rb.velocity;

        rb.acceleration = {0, 0};

    }


}
void SPhysics::Shutdown(){
  
}