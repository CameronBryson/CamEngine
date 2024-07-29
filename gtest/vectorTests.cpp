#include <gtest/gtest.h>
#include "../sources/Vectors.hpp"

// Vec2 Tests
TEST(Vec2Tests, DefaultConstructor) {
    Vec2 v;
    ASSERT_EQ(v.x, 0);
    ASSERT_EQ(v.y, 0);
}

TEST(Vec2Tests, ParameterizedConstructor) {
    Vec2 v(1.0f, 2.0f);
    ASSERT_EQ(v.x, 1.0f);
    ASSERT_EQ(v.y, 2.0f);
}

TEST(Vec2Tests, AdditionOperator) {
    Vec2 v1(1.0f, 2.0f);
    Vec2 v2(3.0f, 4.0f);
    Vec2 result = v1 + v2;
    ASSERT_EQ(result.x, 4.0f);
    ASSERT_EQ(result.y, 6.0f);
}

TEST(Vec2Tests, SubtractionOperator) {
    Vec2 v1(1.0f, 2.0f);
    Vec2 v2(3.0f, 4.0f);
    Vec2 result = v1 - v2;
    ASSERT_EQ(result.x, -2.0f);
    ASSERT_EQ(result.y, -2.0f);
}

TEST(Vec2Tests, MultiplicationOperator) {
    Vec2 v(1.0f, 2.0f);
    Vec2 result = v * 2.0f;
    ASSERT_EQ(result.x, 2.0f);
    ASSERT_EQ(result.y, 4.0f);
}

TEST(Vec2Tests, CompoundAdditionOperator) {
    Vec2 v1(1.0f, 2.0f);
    Vec2 v2(3.0f, 4.0f);
    v1 += v2;
    ASSERT_EQ(v1.x, 4.0f);
    ASSERT_EQ(v1.y, 6.0f);
}

TEST(Vec2Tests, CompoundSubtractionOperator) {
    Vec2 v1(1.0f, 2.0f);
    Vec2 v2(3.0f, 4.0f);
    v1 -= v2;
    ASSERT_EQ(v1.x, -2.0f);
    ASSERT_EQ(v1.y, -2.0f);
}

TEST(Vec2Tests, CompoundMultiplicationOperator) {
    Vec2 v(1.0f, 2.0f);
    v *= 2.0f;
    ASSERT_EQ(v.x, 2.0f);
    ASSERT_EQ(v.y, 4.0f);
}

TEST(Vec2Tests, Length) {
    Vec2 v(3.0f, 4.0f);
    ASSERT_EQ(v.length(), 5.0f);
}

TEST(Vec2Tests, LengthSquared) {
    Vec2 v(3.0f, 4.0f);
    ASSERT_EQ(v.lengthSquared(), 25.0f);
}

TEST(Vec2Tests, Normalized) {
    Vec2 v(3.0f, 4.0f);
    Vec2 result = v.normalized();
    ASSERT_NEAR(result.x, 0.6f, 1e-5);
    ASSERT_NEAR(result.y, 0.8f, 1e-5);
}

// Vec3 Tests
TEST(Vec3Tests, DefaultConstructor) {
    Vec3 v;
    ASSERT_EQ(v.x, 0);
    ASSERT_EQ(v.y, 0);
    ASSERT_EQ(v.z, 0);
}

TEST(Vec3Tests, ParameterizedConstructor) {
    Vec3 v(1.0f, 2.0f, 3.0f);
    ASSERT_EQ(v.x, 1.0f);
    ASSERT_EQ(v.y, 2.0f);
    ASSERT_EQ(v.z, 3.0f);
}

TEST(Vec3Tests, AdditionOperator) {
    Vec3 v1(1.0f, 2.0f, 3.0f);
    Vec3 v2(4.0f, 5.0f, 6.0f);
    Vec3 result = v1 + v2;
    ASSERT_EQ(result.x, 5.0f);
    ASSERT_EQ(result.y, 7.0f);
    ASSERT_EQ(result.z, 9.0f);
}

TEST(Vec3Tests, SubtractionOperator) {
    Vec3 v1(1.0f, 2.0f, 3.0f);
    Vec3 v2(4.0f, 5.0f, 6.0f);
    Vec3 result = v1 - v2;
    ASSERT_EQ(result.x, -3.0f);
    ASSERT_EQ(result.y, -3.0f);
    ASSERT_EQ(result.z, -3.0f);
}

TEST(Vec3Tests, MultiplicationOperator) {
    Vec3 v(1.0f, 2.0f, 3.0f);
    Vec3 result = v * 2.0f;
    ASSERT_EQ(result.x, 2.0f);
    ASSERT_EQ(result.y, 4.0f);
    ASSERT_EQ(result.z, 6.0f);
}

TEST(Vec3Tests, CompoundAdditionOperator) {
    Vec3 v1(1.0f, 2.0f, 3.0f);
    Vec3 v2(4.0f, 5.0f, 6.0f);
    v1 += v2;
    ASSERT_EQ(v1.x, 5.0f);
    ASSERT_EQ(v1.y, 7.0f);
    ASSERT_EQ(v1.z, 9.0f);
}

TEST(Vec3Tests, CompoundSubtractionOperator) {
    Vec3 v1(1.0f, 2.0f, 3.0f);
    Vec3 v2(4.0f, 5.0f, 6.0f);
    v1 -= v2;
    ASSERT_EQ(v1.x, -3.0f);
    ASSERT_EQ(v1.y, -3.0f);
    ASSERT_EQ(v1.z, -3.0f);
}

TEST(Vec3Tests, CompoundMultiplicationOperator) {
    Vec3 v(1.0f, 2.0f, 3.0f);
    v *= 2.0f;
    ASSERT_EQ(v.x, 2.0f);
    ASSERT_EQ(v.y, 4.0f);
    ASSERT_EQ(v.z, 6.0f);
}

TEST(Vec3Tests, Length) {
    Vec3 v(1.0f, 2.0f, 2.0f);
    ASSERT_EQ(v.length(), 3.0f);
}

TEST(Vec3Tests, LengthSquared) {
    Vec3 v(1.0f, 2.0f, 2.0f);
    ASSERT_EQ(v.lengthSquared(), 9.0f);
}

TEST(Vec3Tests, Normalized) {
    Vec3 v(1.0f, 2.0f, 2.0f);
    Vec3 result = v.normalized();
    ASSERT_NEAR(result.x, 1.0f / 3.0f, 1e-5);
    ASSERT_NEAR(result.y, 2.0f / 3.0f, 1e-5);
    ASSERT_NEAR(result.z, 2.0f / 3.0f, 1e-5);
}

// Vec4 Tests
TEST(Vec4Tests, DefaultConstructor) {
    Vec4 v;
    ASSERT_EQ(v.x, 0);
    ASSERT_EQ(v.y, 0);
    ASSERT_EQ(v.z, 0);
    ASSERT_EQ(v.w, 0);
}

TEST(Vec4Tests, ParameterizedConstructor) {
    Vec4 v(1.0f, 2.0f, 3.0f, 4.0f);
    ASSERT_EQ(v.x, 1.0f);
    ASSERT_EQ(v.y, 2.0f);
    ASSERT_EQ(v.z, 3.0f);
    ASSERT_EQ(v.w, 4.0f);
}

TEST(Vec4Tests, AdditionOperator) {
    Vec4 v1(1.0f, 2.0f, 3.0f, 4.0f);
    Vec4 v2(5.0f, 6.0f, 7.0f, 8.0f);
    Vec4 result = v1 + v2;
    ASSERT_EQ(result.x, 6.0f);
    ASSERT_EQ(result.y, 8.0f);
    ASSERT_EQ(result.z, 10.0f);
    ASSERT_EQ(result.w, 12.0f);
}

TEST(Vec4Tests, SubtractionOperator) {
    Vec4 v1(1.0f, 2.0f, 3.0f, 4.0f);
    Vec4 v2(5.0f, 6.0f, 7.0f, 8.0f);
    Vec4 result = v1 - v2;
    ASSERT_EQ(result.x, -4.0f);
    ASSERT_EQ(result.y, -4.0f);
    ASSERT_EQ(result.z, -4.0f);
    ASSERT_EQ(result.w, -4.0f);
}

TEST(Vec4Tests, MultiplicationOperator) {
    Vec4 v(1.0f, 2.0f, 3.0f, 4.0f);
    Vec4 result = v * 2.0f;
    ASSERT_EQ(result.x, 2.0f);
    ASSERT_EQ(result.y, 4.0f);
    ASSERT_EQ(result.z, 6.0f);
    ASSERT_EQ(result.w, 8.0f);
}

TEST(Vec4Tests, CompoundAdditionOperator) {
    Vec4 v1(1.0f, 2.0f, 3.0f, 4.0f);
    Vec4 v2(5.0f, 6.0f, 7.0f, 8.0f);
    v1 += v2;
    ASSERT_EQ(v1.x, 6.0f);
    ASSERT_EQ(v1.y, 8.0f);
    ASSERT_EQ(v1.z, 10.0f);
    ASSERT_EQ(v1.w, 12.0f);
}