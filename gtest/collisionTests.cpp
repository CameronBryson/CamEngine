#include <gtest/gtest.h>
#include "../sources/SCollision.hpp"
#include "../sources/Registry.hpp"
#include "../sources/Components.hpp"

TEST(CollisionTests, AABBAABBIntersectionTest) {
    Registry registry;
    unsigned short entity1 = registry.createEntity();
    unsigned short entity2 = registry.createEntity();

    registry.createSparseSet<CAABB>();
    registry.createSparseSet<CTransform>();

    registry.addComponent<CAABB>(entity1, CAABB{Vec3(1, 1, 1)});
    registry.addComponent<CTransform>(entity1, CTransform{Vec3(0, 0, 0)});

    registry.addComponent<CAABB>(entity2, CAABB{Vec3(1, 1, 1)});
    registry.addComponent<CTransform>(entity2, CTransform{Vec3(1, 1, 1)});

    registry.ProcessCommands();

    ASSERT_TRUE(SCollision::intersectsAABBInAABB(
            registry.getComponent<CAABB>(entity1),
            registry.getComponent<CAABB>(entity2),
            registry.getComponent<CTransform>(entity1),
            registry.getComponent<CTransform>(entity2)
                    ));
}

TEST(CollisionTests, PointAABBIntersectionTest) {
    Registry registry;
    unsigned short entity = registry.createEntity();

    registry.createSparseSet<CAABB>();
    registry.createSparseSet<CTransform>();

    registry.addComponent<CAABB>(entity, CAABB{Vec3(1, 1, 1)});
    registry.addComponent<CTransform>(entity, CTransform{Vec3(0, 0, 0)});

    registry.ProcessCommands();

    Vec3 point(0.5, 0.5, 0.5);
    ASSERT_TRUE(SCollision::intersectsPointInAABB(
            point,
            registry.getComponent<CAABB>(entity),
            registry.getComponent<CTransform>(entity)
                    ));
}

TEST(CollisionTests, SphereAABBIntersectionTest) {
    Registry registry;
    unsigned short entity1 = registry.createEntity();
    unsigned short entity2 = registry.createEntity();

    registry.createSparseSet<CAABB>();
    registry.createSparseSet<CSphere>();
    registry.createSparseSet<CTransform>();

    registry.addComponent<CAABB>(entity1, CAABB{Vec3(1, 1, 1)});
    registry.addComponent<CTransform>(entity1, CTransform{Vec3(0, 0, 0)});

    registry.addComponent<CSphere>(entity2, CSphere{1.0f});
    registry.addComponent<CTransform>(entity2, CTransform{Vec3(0.5, 0.5, 0.5)});

    registry.ProcessCommands();

    ASSERT_TRUE(SCollision::intersectsSphereInAABB(
            registry.getComponent<CSphere>(entity2),
            registry.getComponent<CAABB>(entity1),
            registry.getComponent<CTransform>(entity2),
            registry.getComponent<CTransform>(entity1)
                    ));
}

TEST(CollisionTests, SphereSphereIntersectionTest) {
    Registry registry;
    unsigned short entity1 = registry.createEntity();
    unsigned short entity2 = registry.createEntity();

    registry.createSparseSet<CSphere>();
    registry.createSparseSet<CTransform>();

    registry.addComponent<CSphere>(entity1, CSphere{1.0f});
    registry.addComponent<CTransform>(entity1, CTransform{Vec3(0, 0, 0)});

    registry.addComponent<CSphere>(entity2, CSphere{1.0f});
    registry.addComponent<CTransform>(entity2, CTransform{Vec3(1, 1, 1)});

    registry.ProcessCommands();

    ASSERT_TRUE(SCollision::intersectsSphereInSphere(
            registry.getComponent<CSphere>(entity1),
            registry.getComponent<CSphere>(entity2),
            registry.getComponent<CTransform>(entity1),
            registry.getComponent<CTransform>(entity2)
                    ));

}
TEST(CollisionTests, AABBAABBNonIntersectionTest) {
    Registry registry;
    unsigned short entity1 = registry.createEntity();
    unsigned short entity2 = registry.createEntity();

    registry.createSparseSet<CAABB>();
    registry.createSparseSet<CTransform>();

    registry.addComponent<CAABB>(entity1, CAABB{Vec3(1, 1, 1)});
    registry.addComponent<CTransform>(entity1, CTransform{Vec3(0, 0, 0)});

    registry.addComponent<CAABB>(entity2, CAABB{Vec3(1, 1, 1)});
    registry.addComponent<CTransform>(entity2, CTransform{Vec3(3, 3, 3)});

    registry.ProcessCommands();

    ASSERT_FALSE(SCollision::intersectsAABBInAABB(
            registry.getComponent<CAABB>(entity1),
            registry.getComponent<CAABB>(entity2),
            registry.getComponent<CTransform>(entity1),
            registry.getComponent<CTransform>(entity2)
                    ));
}

TEST(CollisionTests, PointAABBNonIntersectionTest) {
    Registry registry;
    unsigned short entity = registry.createEntity();

    registry.createSparseSet<CAABB>();
    registry.createSparseSet<CTransform>();

    registry.addComponent<CAABB>(entity, CAABB{Vec3(1, 1, 1)});
    registry.addComponent<CTransform>(entity, CTransform{Vec3(0, 0, 0)});

    registry.ProcessCommands();

    Vec3 point(2, 2, 2);
    ASSERT_FALSE(SCollision::intersectsPointInAABB(
            point,
            registry.getComponent<CAABB>(entity),
            registry.getComponent<CTransform>(entity)
                    ));
}

TEST(CollisionTests, SphereAABBNonIntersectionTest) {
    Registry registry;
    unsigned short entity1 = registry.createEntity();
    unsigned short entity2 = registry.createEntity();

    registry.createSparseSet<CAABB>();
    registry.createSparseSet<CSphere>();
    registry.createSparseSet<CTransform>();

    registry.addComponent<CAABB>(entity1, CAABB{Vec3(1, 1, 1)});
    registry.addComponent<CTransform>(entity1, CTransform{Vec3(0, 0, 0)});

    registry.addComponent<CSphere>(entity2, CSphere{1.0f});
    registry.addComponent<CTransform>(entity2, CTransform{Vec3(3, 3, 3)});

    registry.ProcessCommands();

    ASSERT_FALSE(SCollision::intersectsSphereInAABB(
            registry.getComponent<CSphere>(entity2),
            registry.getComponent<CAABB>(entity1),
            registry.getComponent<CTransform>(entity2),
            registry.getComponent<CTransform>(entity1)
                    ));
}

TEST(CollisionTests, SphereSphereNonIntersectionTest) {
    Registry registry;
    unsigned short entity1 = registry.createEntity();
    unsigned short entity2 = registry.createEntity();

    registry.createSparseSet<CSphere>();
    registry.createSparseSet<CTransform>();

    registry.addComponent<CSphere>(entity1, CSphere{1.0f});
    registry.addComponent<CTransform>(entity1, CTransform{Vec3(0, 0, 0)});

    registry.addComponent<CSphere>(entity2, CSphere{1.0f});
    registry.addComponent<CTransform>(entity2, CTransform{Vec3(3, 3, 3)});

    registry.ProcessCommands();

    ASSERT_FALSE(SCollision::intersectsSphereInSphere(
            registry.getComponent<CSphere>(entity1), registry.getComponent<CSphere>(entity2),
            registry.getComponent<CTransform>(entity1), registry.getComponent<CTransform>(entity2)));
}