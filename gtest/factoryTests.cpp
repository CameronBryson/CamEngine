#include "../sources/Components.hpp"
#include "../sources/Factory.hpp"
#include "../sources/Registry.hpp"
#include <gtest/gtest.h>

TEST(FactoryTests, CreatePlayerTest)
{
    registry registry;
    registry.create_sparse_set<c_player>();
    registry.create_sparse_set<c_transform>();
    registry.create_sparse_set<c_velocity>();
    registry.create_sparse_set<c_rigid_body>();
    registry.create_sparse_set<c_aabb>();
    unsigned short player_id = factory::create_player(registry);
    registry.process_commands();
    ASSERT_TRUE(registry.has_component<c_player>(player_id));
    ASSERT_TRUE(registry.has_component<c_transform>(player_id));
    ASSERT_TRUE(registry.has_component<c_velocity>(player_id));
    ASSERT_TRUE(registry.has_component<c_rigid_body>(player_id));
    ASSERT_TRUE(registry.has_component<c_aabb>(player_id));
}

TEST(FactoryTests, PlayerComponentsDefaultValuesTest)
{
    registry registry;
    registry.create_sparse_set<c_player>();
    registry.create_sparse_set<c_transform>();
    registry.create_sparse_set<c_velocity>();
    registry.create_sparse_set<c_rigid_body>();
    registry.create_sparse_set<c_aabb>();
    unsigned short player_id = factory::create_player(registry);
    registry.process_commands();

    const c_player &player = registry.get_component<c_player>(player_id);
    const c_transform &transform = registry.get_component<c_transform>(player_id);
    const c_velocity &velocity = registry.get_component<c_velocity>(player_id);
    const c_rigid_body &rigidBody = registry.get_component<c_rigid_body>(player_id);
    const c_aabb &aabb = registry.get_component<c_aabb>(player_id);

    // Check default values
    ASSERT_EQ(rigidBody.drag, 0.9f);
    ASSERT_EQ(aabb.extents.x, 1.0f);
    ASSERT_EQ(aabb.extents.y, 1.0f);
    ASSERT_EQ(aabb.extents.z, 1.0f);
}