#include <gtest/gtest.h>
#include "../sources/Registry.hpp"
#include "../sources/Components.hpp"

class RegistryTests : public ::testing::Test {
  protected:
    registry reg;

    void SetUp() override {
        reg.create_sparse_set<c_player>();
        reg.create_sparse_set<c_transform>();
        reg.create_sparse_set<c_velocity>();
        reg.create_sparse_set<c_dynamic_body>();
        reg.create_sparse_set<c_quad>();
    }
};

TEST_F(RegistryTests, CreateEntity) {
    unsigned short entity_id = reg.create_entity();
    EXPECT_NE(entity_id, 0);
}

TEST_F(RegistryTests, AddComponent) {
    unsigned short entity_id = reg.create_entity();
    c_transform transform{.position = {1, 2, 3}, .rotation = {0, 0, 0}, .scale = {1, 1, 1}};
    reg.add_component<c_transform>(entity_id, transform);
    reg.process_commands();
    EXPECT_TRUE(reg.has_component<c_transform>(entity_id));
}

TEST_F(RegistryTests, GetComponent) {
    unsigned short entity_id = reg.create_entity();
    c_transform transform{.position = {1, 2, 3}, .rotation = {0, 0, 0}, .scale = {1, 1, 1}};
    reg.add_component<c_transform>(entity_id, transform);
    reg.process_commands();
    const auto& retrieved_transform = reg.get_component<c_transform>(entity_id);
    EXPECT_FLOAT_EQ(retrieved_transform.position.x, 1.0f);
    EXPECT_FLOAT_EQ(retrieved_transform.position.y, 2.0f);
    EXPECT_FLOAT_EQ(retrieved_transform.position.z, 3.0f);
}

TEST_F(RegistryTests, RemoveComponent) {
    unsigned short entity_id = reg.create_entity();
    c_transform transform{.position = {1, 2, 3}, .rotation = {0, 0, 0}, .scale = {1, 1, 1}};
    reg.add_component<c_transform>(entity_id, transform);
    reg.remove_component<c_transform>(entity_id);
    EXPECT_FALSE(reg.has_component<c_transform>(entity_id));
}

TEST_F(RegistryTests, DeleteEntity) {
    unsigned short entity_id = reg.create_entity();
    reg.delete_entity(entity_id);
    reg.process_commands();
    EXPECT_FALSE(std::find(reg.get_entity_ids<>().begin(), reg.get_entity_ids<>().end(), entity_id) != reg.get_entity_ids<>().end());
}

TEST_F(RegistryTests, ProcessCommands) {
    unsigned short entity_id = reg.create_entity();
    c_transform transform{.position = {1, 2, 3}, .rotation = {0, 0, 0}, .scale = {1, 1, 1}};
    reg.add_component<c_transform>(entity_id, transform);
    reg.process_commands();
    EXPECT_TRUE(reg.has_component<c_transform>(entity_id));
}