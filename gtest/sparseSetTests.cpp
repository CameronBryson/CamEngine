#include <gtest/gtest.h>
#include "../sources/SparseSet.hpp"

TEST(SparseSetTests, AddItemTest) {
    SparseSet<int> sparseSet;
    sparseSet.addItem(1, 42);
    ASSERT_TRUE(sparseSet.hasItem(1));
}

TEST(SparseSetTests, RemoveItemTest) {
    SparseSet<int> sparseSet;
    sparseSet.addItem(1, 42);
    sparseSet.removeItem(1);
    ASSERT_FALSE(sparseSet.hasItem(1));
}

TEST(SparseSetTests, HasItemTest) {
    SparseSet<int> sparseSet;
    sparseSet.addItem(1, 42);
    ASSERT_TRUE(sparseSet.hasItem(1));
    ASSERT_FALSE(sparseSet.hasItem(2));
}

TEST(SparseSetTests, GetItemTest) {
    SparseSet<int> sparseSet;
    sparseSet.addItem(1, 42);
    ASSERT_EQ(sparseSet.getItem(1), 42);
}

TEST(SparseSetTests, GetSizeTest) {
    SparseSet<int> sparseSet;
    sparseSet.addItem(1, 42);
    sparseSet.addItem(2, 43);
    ASSERT_EQ(sparseSet.getSize(), 2);
}

TEST(SparseSetTests, GetIntersectionTest) {
    SparseSet<int> sparseSet;
    sparseSet.addItem(1, 42);
    sparseSet.addItem(2, 43);
    std::vector<unsigned short> other = {2, 3};
    auto intersection = sparseSet.getIntersection(other);
    ASSERT_EQ(intersection.size(), 1);
    ASSERT_EQ(intersection[0], 2);
}