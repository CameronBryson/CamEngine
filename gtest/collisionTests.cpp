#include <gtest/gtest.h>
#include "../sources/SCollision.hpp"
#include "../sources/Components.hpp"
#include "../sources/Registry.hpp"

class CollisionTests : public ::testing::Test {
  protected:
    registry reg;
    c_transform transform1, transform2;
    c_quad aabb1, aabb2, obb1, obb2;
    c_sphere sphere1, sphere2;

    void SetUp() override {
        // Initialize components
        aabb1.extents = glm::vec3(2.0f, 2.0f, 2.0f);
        aabb2.extents = glm::vec3(2.0f, 2.0f, 2.0f);
        obb1.extents = glm::vec3(2.0f, 2.0f, 2.0f);
        obb2.extents = glm::vec3(2.0f, 2.0f, 2.0f);
        sphere1.radius = 1.0f;
        sphere2.radius = 1.0f;

        transform1.position = glm::vec3(0.0f, 0.0f, 0.0f);
        transform2.position = glm::vec3(1.0f, 1.0f, 1.0f);
    }
};

TEST_F(CollisionTests, AABBvsAABB) {
    auto result = s_collision::intersects_aabb_in_aabb(aabb1, aabb2, transform1, transform2);
    EXPECT_TRUE(result.intersects);
}

TEST_F(CollisionTests, SpherevsAABB) {
    auto result = s_collision::intersects_sphere_in_aabb(sphere1, aabb1, transform1, transform2);
    EXPECT_TRUE(result.intersects);
}

TEST_F(CollisionTests, SpherevsSphere) {
    auto result = s_collision::intersects_sphere_in_sphere(sphere1, sphere2, transform1, transform2);
    EXPECT_TRUE(result.intersects);
}

TEST_F(CollisionTests, OBBvsOBB) {
    auto result = s_collision::intersects_obb_in_obb(obb1, obb2, transform1, transform2);
    EXPECT_TRUE(result.intersects);
}

TEST_F(CollisionTests, OBBvsAABB) {
    auto result = s_collision::intersects_obb_in_aabb(obb1, aabb1, transform1, transform2);
    EXPECT_TRUE(result.intersects);
}

TEST_F(CollisionTests, OBBvsSphere) {
    auto result = s_collision::intersects_obb_in_sphere(obb1, sphere1, transform1, transform2);
    EXPECT_TRUE(result.intersects);
}