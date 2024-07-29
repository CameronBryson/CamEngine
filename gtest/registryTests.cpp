#include <gtest/gtest.h>
#include <memory>
#include "../sources/Registry.hpp"

TEST(RegistryTests, CreateEntityTest) {
    Registry registry;
    for (int i = 0; i < Settings::MAX_ENTITIES - 1; i++) {
        registry.createEntity();
    }
    ASSERT_EQ(registry.getEntityIDS().size(), Settings::MAX_ENTITIES - 1);
}

TEST(RegistryTests, DeleteEntityTest) {
    Registry registry;
    for (int i = 0; i < Settings::MAX_ENTITIES - 1; i++) {
        registry.createEntity();
    }
    for (int i = 1; i < Settings::MAX_ENTITIES; i++) {
        registry.deleteEntity(i);
    }
    registry.ProcessCommands();
    ASSERT_EQ(registry.getEntityIDS().size(), 0);
}

TEST(RegistryTests, CreateSparseSetTest) {
    Registry registry;
    registry.createSparseSet<int>();
    ASSERT_TRUE(registry.hasSparseSet<int>());
}

TEST(RegistryTests, AddComponentTest) {
    Registry registry;
    registry.createSparseSet<int>();
    unsigned short entity = registry.createEntity();
    registry.addComponent<int>(entity, 42);
    registry.ProcessCommands();
    ASSERT_TRUE(registry.hasComponent<int>(entity));
}

TEST(RegistryTests, GetComponentTest) {
    Registry registry;
    registry.createSparseSet<int>();

    unsigned short entity = registry.createEntity();
    registry.addComponent<int>(entity, 42);
    registry.ProcessCommands();
    ASSERT_EQ(registry.getComponent<int>(entity), 42);
}

TEST(RegistryTests, RemoveComponentTest) {
    Registry registry;
    registry.createSparseSet<int>();
    unsigned short entity = registry.createEntity();
    registry.addComponent<int>(entity, 42);
    registry.ProcessCommands();
    registry.removeComponent<int>(entity);
    registry.ProcessCommands();
    ASSERT_FALSE(registry.hasComponent<int>(entity));
}

TEST(RegistryTests, MultipleComponentsTest) {
    Registry registry;
    registry.createSparseSet<int>();
    registry.createSparseSet<float>();
    unsigned short entity = registry.createEntity();
    registry.addComponent<int>(entity, 42);
    registry.addComponent<float>(entity, 3.14f);
    registry.ProcessCommands();
    ASSERT_EQ(registry.getComponent<int>(entity), 42);
    ASSERT_EQ(registry.getComponent<float>(entity), 3.14f);
}