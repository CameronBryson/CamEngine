#include "../sources/SparseSet.hpp"
#include <gtest/gtest.h>

TEST(SparseSetTests, AddItemTest)
{
    sparse_set<int> sparseSet;
    sparseSet.add_item(1, 42);
    ASSERT_TRUE(sparseSet.has_item(1));
}

TEST(SparseSetTests, RemoveItemTest)
{
    sparse_set<int> sparseSet;
    sparseSet.add_item(1, 42);
    sparseSet.remove_item(1);
    ASSERT_FALSE(sparseSet.has_item(1));
}

TEST(SparseSetTests, HasItemTest)
{
    sparse_set<int> sparseSet;
    sparseSet.add_item(1, 42);
    ASSERT_TRUE(sparseSet.has_item(1));
    ASSERT_FALSE(sparseSet.has_item(2));
}

TEST(SparseSetTests, GetItemTest)
{
    sparse_set<int> sparseSet;
    sparseSet.add_item(1, 42);
    ASSERT_EQ(sparseSet.get_item(1), 42);
}

TEST(SparseSetTests, GetSizeTest)
{
    sparse_set<int> sparseSet;
    sparseSet.add_item(1, 42);
    sparseSet.add_item(2, 43);
    ASSERT_EQ(sparseSet.get_size(), 2);
}

TEST(SparseSetTests, GetIntersectionTest)
{
    sparse_set<int> sparseSet;
    sparseSet.add_item(1, 42);
    sparseSet.add_item(2, 43);
    std::vector<unsigned short> other = {2, 3};
    auto intersection = sparseSet.get_intersection(other);
    ASSERT_EQ(intersection.size(), 1);
    ASSERT_EQ(intersection[0], 2);
}