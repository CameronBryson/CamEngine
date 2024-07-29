#include <gtest/gtest.h>
#include "../sources/Factory.hpp"
#include "../sources/Registry.hpp"
#include "../sources/Components.hpp"

TEST(FactoryTests, CreatePlayerTest) {
    Registry registry;
    registry.createSparseSet<CPlayer>();
    registry.createSparseSet<CTransform>();
    registry.createSparseSet<CVelocity>();
    registry.createSparseSet<CRigidBody>();
    registry.createSparseSet<CAABB>();
    unsigned short playerID = Factory::CreatePlayer(registry);
    registry.ProcessCommands();
    ASSERT_TRUE(registry.hasComponent<CPlayer>(playerID));
    ASSERT_TRUE(registry.hasComponent<CTransform>(playerID));
    ASSERT_TRUE(registry.hasComponent<CVelocity>(playerID));
    ASSERT_TRUE(registry.hasComponent<CRigidBody>(playerID));
    ASSERT_TRUE(registry.hasComponent<CAABB>(playerID));
}

TEST(FactoryTests, PlayerComponentsDefaultValuesTest) {
    Registry registry;
    registry.createSparseSet<CPlayer>();
    registry.createSparseSet<CTransform>();
    registry.createSparseSet<CVelocity>();
    registry.createSparseSet<CRigidBody>();
    registry.createSparseSet<CAABB>();
    unsigned short playerID = Factory::CreatePlayer(registry);
    registry.ProcessCommands();

    const CPlayer& player = registry.getComponent<CPlayer>(playerID);
    const CTransform& transform = registry.getComponent<CTransform>(playerID);
    const CVelocity& velocity = registry.getComponent<CVelocity>(playerID);
    const CRigidBody& rigidBody = registry.getComponent<CRigidBody>(playerID);
    const CAABB& aabb = registry.getComponent<CAABB>(playerID);

    // Check default values
    ASSERT_EQ(rigidBody.drag, 0.9f);
    ASSERT_EQ(aabb.extents.x, 1.0f);
    ASSERT_EQ(aabb.extents.y, 1.0f);
    ASSERT_EQ(aabb.extents.z, 1.0f);
}