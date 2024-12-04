#include "SPhysics.hpp"
#include "Engine/Components.hpp"
#include "Engine/Base/BaseScene.hpp"
#include <cmath>
#include <mutex>
#include <vector>

SPhysics::SPhysics(BaseScene* scene) : mScene(scene)
{
}

void SPhysics::init()
{
}

void SPhysics::update(const float dt)
{
    updateDynamicBodies(dt);
}


void SPhysics::updateDynamicBodies(float dt) const
{
    auto & dynamic_bodies = mScene->getSparseSet<CDynamicBody>();
    auto & transforms = mScene->getSparseSet<CTransform>();
    auto & repeat_acceleration = mScene->getSparseSet<CRepeatAcceleration>();
    const auto ids = mScene->getEntityIDs<CTransform,CDynamicBody>();
    for( const auto id : ids )
    {
		auto& [elasticity, drag, velocity, acceleration, angular_drag, angular_velocity, angular_acceleration] =
		    dynamic_bodies.get_item(id);
        auto & [position, rotation, scale] = transforms.get_item(id);
        if(repeat_acceleration.hasItem(id))
        {
            acceleration+= repeat_acceleration.get_item(id).acceleration;
            angular_acceleration = repeat_acceleration.get_item(id).angularAcceleration;
        }

        velocity += acceleration * dt;
		//printf("Drag %f\n", drag);
		//printf("Before Velo: %f, %f, %f\n", velocity.x, velocity.y, velocity.z);
		velocity *= std::pow(1-drag, dt);
		//printf("After Velo: %f, %f, %f\n", velocity.x, velocity.y, velocity.z);
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