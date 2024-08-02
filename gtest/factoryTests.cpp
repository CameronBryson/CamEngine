#include <gtest/gtest.h>
#include "../sources/Factory.hpp"
#include "../sources/Registry.hpp"

class FactoryTests : public ::testing::Test {
  protected:
    registry reg;

    void SetUp() override {
        reg.create_sparse_set<c_player>();
        reg.create_sparse_set<c_transform>();
        reg.create_sparse_set<c_velocity>();
        reg.create_sparse_set<c_rigid_body>();
        reg.create_sparse_set<c_quad>();
    }
};

TEST_F(FactoryTests, CreatePlayer) {
    unsigned short player_id = factory::create_player(reg);
    reg.process_commands();
    // Verify the player entity has the correct components
    EXPECT_TRUE(reg.has_component<c_player>(player_id));
    EXPECT_TRUE(reg.has_component<c_transform>(player_id));
    EXPECT_TRUE(reg.has_component<c_velocity>(player_id));
    EXPECT_TRUE(reg.has_component<c_rigid_body>(player_id));
    EXPECT_TRUE(reg.has_component<c_quad>(player_id));

    // Verify the values of the components
    const auto& transform = reg.get_component<c_transform>(player_id);
    EXPECT_EQ(transform.position, vec3(0, 0, 0));
    EXPECT_EQ(transform.rotation, vec3(0, 0, 90));
    EXPECT_EQ(transform.scale, vec3(1, 1, 1));

    const auto& rigid_body = reg.get_component<c_rigid_body>(player_id);
    EXPECT_FLOAT_EQ(rigid_body.drag, 0.99f);

    const auto& quad = reg.get_component<c_quad>(player_id);
    EXPECT_EQ(quad.extents, vec3(0.1f, 0.2f, 0.3f));
}