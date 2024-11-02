#include "SPhysics.hpp"
#include "Components.hpp"
#include "Engine/Registry.hpp"
#include <cmath>
#include <mutex>
#include <vector>

void SPhysics::update(const Registry & registry, const float dt)
{
    updateDynamicBodies(registry, dt);
}


void SPhysics::updateDynamicBodies(const Registry & registry, float dt)
{
    auto & dynamic_bodies = registry.getSparseSet<CDynamicBody>();
    auto & transforms = registry.getSparseSet<CTransform>();
    auto & repeat_acceleration = registry.getSparseSet<CRepeatAcceleration>();
    const auto ids = registry.getEntityIDs<CTransform,CDynamicBody>();
    for( const auto id : ids )
    {
        auto & [drag, elasticity,  velocity ,acceleration, angular_drag, angular_velocity, angular_acceleration] = dynamic_bodies.get_item(id);
        auto & [position, rotation, scale] = transforms.get_item(id);
        if(repeat_acceleration.hasItem(id))
        {
            acceleration+= repeat_acceleration.get_item(id).acceleration;
            angular_acceleration = repeat_acceleration.get_item(id).angularAcceleration;
        }
        velocity += acceleration * dt;
        velocity *= std::pow(1 - drag, dt);
        position += velocity * dt;
        acceleration = { 0, 0, 0 };

        angular_velocity += angular_acceleration * dt;
        angular_velocity *= std::pow(1 - angular_drag, dt);
        rotation += angular_velocity*dt;
        angular_acceleration = { 0, 0, 0 };
    }
}

void SPhysics::shutdown()
{
}