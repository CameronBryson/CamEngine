#include "../sources/Components.hpp"
#include "../sources/Registry.hpp"
#include "../sources/SCollision.hpp"
#include <gtest/gtest.h>

TEST(CollisionTests, AABBAABBIntersectionTest)
{
    registry registry;
    unsigned short entity1 = registry.create_entity();
    unsigned short entity2 = registry.create_entity();
    registry.create_sparse_set<c_quad>();
    registry.create_sparse_set<c_transform>();

    registry.add_component<c_quad>(entity1, c_quad{vec3(1, 1, 1)});
    registry.add_component<c_transform>(entity1, c_transform{vec3(0, 0, 0)});

    registry.add_component<c_quad>(entity2, c_quad{vec3(1, 1, 1)});
    registry.add_component<c_transform>(entity2, c_transform{vec3(1, 1, 1)});

    registry.process_commands();

    ASSERT_TRUE(s_collision::intersects_aabb_in_aabb(
        registry.get_component<c_quad>(entity1), registry.get_component<c_quad>(entity2),
        registry.get_component<c_transform>(entity1), registry.get_component<c_transform>(entity2)));
}

TEST(CollisionTests, PointAABBIntersectionTest)
{
    registry registry;
    unsigned short entity = registry.create_entity();

    registry.create_sparse_set<c_quad>();
    registry.create_sparse_set<c_transform>();

    registry.add_component<c_quad>(entity, c_quad{vec3(1, 1, 1)});
    registry.add_component<c_transform>(entity, c_transform{vec3(0, 0, 0)});

    registry.process_commands();

    vec3 point(0.5, 0.5, 0.5);
    ASSERT_TRUE(s_collision::intersects_point_in_aabb(point, registry.get_component<c_quad>(entity),
                                                      registry.get_component<c_transform>(entity)));
}

TEST(CollisionTests, SphereAABBIntersectionTest)
{
    registry registry;
    unsigned short entity1 = registry.create_entity();
    unsigned short entity2 = registry.create_entity();

    registry.create_sparse_set<c_quad>();
    registry.create_sparse_set<c_sphere>();
    registry.create_sparse_set<c_transform>();

    registry.add_component<c_quad>(entity1, c_quad{vec3(1, 1, 1)});
    registry.add_component<c_transform>(entity1, c_transform{vec3(0, 0, 0)});

    registry.add_component<c_sphere>(entity2, c_sphere{1.0f});
    registry.add_component<c_transform>(entity2, c_transform{vec3(0.5, 0.5, 0.5)});

    registry.process_commands();

    ASSERT_TRUE(s_collision::intersects_sphere_in_aabb(
        registry.get_component<c_sphere>(entity2), registry.get_component<c_quad>(entity1),
        registry.get_component<c_transform>(entity2), registry.get_component<c_transform>(entity1)));
}

TEST(CollisionTests, SphereSphereIntersectionTest)
{
    registry registry;
    unsigned short entity1 = registry.create_entity();
    unsigned short entity2 = registry.create_entity();

    registry.create_sparse_set<c_sphere>();
    registry.create_sparse_set<c_transform>();

    registry.add_component<c_sphere>(entity1, c_sphere{1.0f});
    registry.add_component<c_transform>(entity1, c_transform{vec3(0, 0, 0)});

    registry.add_component<c_sphere>(entity2, c_sphere{1.0f});
    registry.add_component<c_transform>(entity2, c_transform{vec3(1, 1, 1)});

    registry.process_commands();

    ASSERT_TRUE(s_collision::intersects_sphere_in_sphere(
        registry.get_component<c_sphere>(entity1), registry.get_component<c_sphere>(entity2),
        registry.get_component<c_transform>(entity1), registry.get_component<c_transform>(entity2)));
}

TEST(CollisionTests, AABBAABBNonIntersectionTest)
{
    registry registry;
    unsigned short entity1 = registry.create_entity();
    unsigned short entity2 = registry.create_entity();

    registry.create_sparse_set<c_quad>();
    registry.create_sparse_set<c_transform>();

    registry.add_component<c_quad>(entity1, c_quad{vec3(1, 1, 1)});
    registry.add_component<c_transform>(entity1, c_transform{vec3(0, 0, 0)});

    registry.add_component<c_quad>(entity2, c_quad{vec3(1, 1, 1)});
    registry.add_component<c_transform>(entity2, c_transform{vec3(3, 3, 3)});

    registry.process_commands();

    ASSERT_FALSE(s_collision::intersects_aabb_in_aabb(
        registry.get_component<c_quad>(entity1), registry.get_component<c_quad>(entity2),
        registry.get_component<c_transform>(entity1), registry.get_component<c_transform>(entity2)));
}

TEST(CollisionTests, PointAABBNonIntersectionTest)
{
    registry registry;
    unsigned short entity = registry.create_entity();

    registry.create_sparse_set<c_quad>();
    registry.create_sparse_set<c_transform>();

    registry.add_component<c_quad>(entity, c_quad{vec3(1, 1, 1)});
    registry.add_component<c_transform>(entity, c_transform{vec3(0, 0, 0)});

    registry.process_commands();

    vec3 point(2, 2, 2);
    ASSERT_FALSE(s_collision::intersects_point_in_aabb(point, registry.get_component<c_quad>(entity),
                                                       registry.get_component<c_transform>(entity)));
}

TEST(CollisionTests, SphereAABBNonIntersectionTest)
{
    registry registry;
    unsigned short entity1 = registry.create_entity();
    unsigned short entity2 = registry.create_entity();

    registry.create_sparse_set<c_quad>();
    registry.create_sparse_set<c_sphere>();
    registry.create_sparse_set<c_transform>();

    registry.add_component<c_quad>(entity1, c_quad{vec3(1, 1, 1)});
    registry.add_component<c_transform>(entity1, c_transform{vec3(0, 0, 0)});

    registry.add_component<c_sphere>(entity2, c_sphere{1.0f});
    registry.add_component<c_transform>(entity2, c_transform{vec3(3, 3, 3)});

    registry.process_commands();

    ASSERT_FALSE(s_collision::intersects_sphere_in_aabb(
        registry.get_component<c_sphere>(entity2), registry.get_component<c_quad>(entity1),
        registry.get_component<c_transform>(entity2), registry.get_component<c_transform>(entity1)));
}

TEST(CollisionTests, SphereSphereNonIntersectionTest)
{
    registry registry;
    unsigned short entity1 = registry.create_entity();
    unsigned short entity2 = registry.create_entity();

    registry.create_sparse_set<c_sphere>();
    registry.create_sparse_set<c_transform>();

    registry.add_component<c_sphere>(entity1, c_sphere{1.0f});
    registry.add_component<c_transform>(entity1, c_transform{vec3(0, 0, 0)});

    registry.add_component<c_sphere>(entity2, c_sphere{1.0f});
    registry.add_component<c_transform>(entity2, c_transform{vec3(3, 3, 3)});

    registry.process_commands();

    ASSERT_FALSE(s_collision::intersects_sphere_in_sphere(
        registry.get_component<c_sphere>(entity1), registry.get_component<c_sphere>(entity2),
        registry.get_component<c_transform>(entity1), registry.get_component<c_transform>(entity2)));
}