#include <gtest/gtest.h>
#include "../sources/SparseSet.hpp"
#include "../sources/Registry.hpp"
#include "../sources/Components.hpp"

class SparseSetTests : public ::testing::Test {
  protected:
    registry reg;

    void SetUp() override {
        reg.create_sparse_set<c_transform>();
    }
};

TEST_F(SparseSetTests, CreateSparseSet) {
    EXPECT_TRUE(reg.has_sparse_set<c_transform>());
}

TEST_F(SparseSetTests, AddComponent) {
    unsigned short entity_id = reg.create_entity();
    c_transform transform{.position = {1, 2, 3}, .rotation = {0, 0, 0}, .scale = {1, 1, 1}};
    reg.add_component<c_transform>(entity_id, transform);
    reg.process_commands();
    EXPECT_TRUE(reg.has_component<c_transform>(entity_id));
    const auto& retrieved_transform = reg.get_component<c_transform>(entity_id);
    EXPECT_EQ(retrieved_transform.position, vec3(1, 2, 3));
}

TEST_F(SparseSetTests, RemoveComponent) {
    unsigned short entity_id = reg.create_entity();
    c_transform transform{.position = {1, 2, 3}, .rotation = {0, 0, 0}, .scale = {1, 1, 1}};
    reg.add_component<c_transform>(entity_id, transform);
    reg.remove_component<c_transform>(entity_id);
    reg.process_commands();

    EXPECT_FALSE(reg.has_component<c_transform>(entity_id));
}

TEST_F(SparseSetTests, GetComponent) {
    unsigned short entity_id = reg.create_entity();
    c_transform transform{.position = {1, 2, 3}, .rotation = {0, 0, 0}, .scale = {1, 1, 1}};
    reg.add_component<c_transform>(entity_id, transform);
    reg.process_commands();
    const auto& retrieved_transform = reg.get_component<c_transform>(entity_id);
    EXPECT_EQ(retrieved_transform.position, vec3(1, 2, 3));
    EXPECT_EQ(retrieved_transform.rotation, vec3(0, 0, 0));
    EXPECT_EQ(retrieved_transform.scale, vec3(1, 1, 1));
}

TEST_F(SparseSetTests, SparseSetSize) {
    unsigned short entity_id1 = reg.create_entity();
    unsigned short entity_id2 = reg.create_entity();
    c_transform transform{.position = {1, 2, 3}, .rotation = {0, 0, 0}, .scale = {1, 1, 1}};
    reg.add_component<c_transform>(entity_id1, transform);
    reg.add_component<c_transform>(entity_id2, transform);
    reg.process_commands();
    const auto& sparse_set = reg.get_sparse_set<c_transform>();
    EXPECT_EQ(sparse_set.get_size(), 2);
}