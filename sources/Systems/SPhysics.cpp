#include "SPhysics.hpp"
#include "Components.hpp"
#include "Engine/Registry.hpp"
#include <cmath>
#include <mutex>
#include <vector>

void s_physics::update(const registry & registry, const float dt)
{
    update_dynamic_bodies(registry, dt);
}


void s_physics::update_dynamic_bodies(const registry & registry, float dt)
{
    auto & dynamic_bodies = registry.get_sparse_set<c_dynamic_body>();
    auto & transforms = registry.get_sparse_set<c_transform>();
    auto & repeat_acceleration = registry.get_sparse_set<c_repeat_acceleration>();
    const auto ids = registry.get_entity_ids<c_transform,c_dynamic_body>();
    for( const auto id : ids )
    {
        auto & [drag, elasticity,  velocity ,acceleration, angular_drag, angular_velocity, angular_acceleration] = dynamic_bodies.get_item(id);
        auto & [position, rotation, scale] = transforms.get_item(id);
        if(repeat_acceleration.has_item(id))
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

void s_physics::shutdown()
{
}