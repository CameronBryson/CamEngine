#include "../sources/Registry.hpp"
#include <gtest/gtest.h>
#include <memory>

TEST(RegistryTests, CreateEntityTest)
{
    registry registry;
    for (int i = 0; i < settings::max_entities - 1; i++)
    {
        registry.create_entity();
    }
    ASSERT_EQ(registry.get_entity_ids().size(), settings::max_entities - 1);
}

TEST(RegistryTests, DeleteEntityTest)
{
    registry registry;
    for (int i = 0; i < settings::max_entities - 1; i++)
    {
        registry.create_entity();
    }
    for (int i = 1; i < settings::max_entities; i++)
    {
        registry.delete_entity(i);
    }
    registry.process_commands();
    ASSERT_EQ(registry.get_entity_ids().size(), 0);
}

TEST(RegistryTests, CreateSparseSetTest)
{
    registry registry;
    registry.create_sparse_set<int>();
    ASSERT_TRUE(registry.has_sparse_set<int>());
}

TEST(RegistryTests, AddComponentTest)
{
    registry registry;
    registry.create_sparse_set<int>();
    unsigned short entity = registry.create_entity();
    registry.add_component<int>(entity, 42);
    registry.process_commands();
    ASSERT_TRUE(registry.has_component<int>(entity));
}

TEST(RegistryTests, GetComponentTest)
{
    registry registry;
    registry.create_sparse_set<int>();

    unsigned short entity = registry.create_entity();
    registry.add_component<int>(entity, 42);
    registry.process_commands();
    ASSERT_EQ(registry.get_component<int>(entity), 42);
}

TEST(RegistryTests, RemoveComponentTest)
{
    registry registry;
    registry.create_sparse_set<int>();
    unsigned short entity = registry.create_entity();
    registry.add_component<int>(entity, 42);
    registry.process_commands();
    registry.remove_component<int>(entity);
    registry.process_commands();
    ASSERT_FALSE(registry.has_component<int>(entity));
}

TEST(RegistryTests, MultipleComponentsTest)
{
    registry registry;
    registry.create_sparse_set<int>();
    registry.create_sparse_set<float>();
    unsigned short entity = registry.create_entity();
    registry.add_component<int>(entity, 42);
    registry.add_component<float>(entity, 3.14f);
    registry.process_commands();
    ASSERT_EQ(registry.get_component<int>(entity), 42);
    ASSERT_EQ(registry.get_component<float>(entity), 3.14f);
}