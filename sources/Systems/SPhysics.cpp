#include "SPhysics.hpp"
#include "Components.hpp"
#include <cmath>
#include <mutex>
#include <vector>

void s_physics::update(const registry & registry, const float dt)
{
    update_kinematic_bodies(registry, dt);
    update_dynamic_bodies(registry, dt);
}

void s_physics::update_kinematic_bodies(const registry & registry, float dt)
{
    auto & positions = registry.get_sparse_set<c_transform>();
    auto & velocities = registry.get_sparse_set<c_velocity>();
    const auto ids = registry.get_entity_ids<c_transform, c_velocity,c_kinematic_body>();

    for( const auto id : ids )
    {
        auto & [position, rotation, scale] = positions.get_item(id);
        auto & [velocity] = velocities.get_item(id);

        position += velocity;
    }
}

void s_physics::update_dynamic_bodies(const registry & registry, float dt)
{
    auto & dynamic_bodies = registry.get_sparse_set<c_dynamic_body>();
    auto & positions = registry.get_sparse_set<c_transform>();
    auto & velocities = registry.get_sparse_set<c_velocity>();
    const auto ids = registry.get_entity_ids<c_transform, c_velocity,c_dynamic_body>();

    for( const auto id : ids )
    {
        auto & [mass,drag,acceleration,gravity,grounded] = dynamic_bodies.get_item(id);
        auto & [position, rotation, scale] = positions.get_item(id);
        auto & [velocity] = velocities.get_item(id);

        if (!grounded) acceleration+=gravity;
        velocity += acceleration * dt;
        velocity *= std::pow(1 - drag, dt);
        position += velocity;
        acceleration = { 0, 0, 0 };
    }
}

void s_physics::shutdown()
{
}