#include "../sources/Engine/Vectors.hpp"
#include <gtest/gtest.h>

// Test vec2 class
TEST(Vec2Tests, Constructors) {
    vec2 v1;
    EXPECT_FLOAT_EQ(v1.x, 0.0f);
    EXPECT_FLOAT_EQ(v1.y, 0.0f);

    vec2 v2(1.0f, 2.0f);
    EXPECT_FLOAT_EQ(v2.x, 1.0f);
    EXPECT_FLOAT_EQ(v2.y, 2.0f);
}

TEST(Vec2Tests, ArithmeticOperators) {
    vec2 v1(1.0f, 2.0f);
    vec2 v2(3.0f, 4.0f);

    vec2 v3 = v1 + v2;
    EXPECT_FLOAT_EQ(v3.x, 4.0f);
    EXPECT_FLOAT_EQ(v3.y, 6.0f);

    vec2 v4 = v1 - v2;
    EXPECT_FLOAT_EQ(v4.x, -2.0f);
    EXPECT_FLOAT_EQ(v4.y, -2.0f);

    vec2 v5 = v1 * 2.0f;
    EXPECT_FLOAT_EQ(v5.x, 2.0f);
    EXPECT_FLOAT_EQ(v5.y, 4.0f);

    vec2 v6 = v1 / 2.0f;
    EXPECT_FLOAT_EQ(v6.x, 0.5f);
    EXPECT_FLOAT_EQ(v6.y, 1.0f);
}

TEST(Vec2Tests, CompoundAssignmentOperators) {
    vec2 v1(1.0f, 2.0f);
    vec2 v2(3.0f, 4.0f);

    v1 += v2;
    EXPECT_FLOAT_EQ(v1.x, 4.0f);
    EXPECT_FLOAT_EQ(v1.y, 6.0f);

    v1 -= v2;
    EXPECT_FLOAT_EQ(v1.x, 1.0f);
    EXPECT_FLOAT_EQ(v1.y, 2.0f);

    v1 *= 2.0f;
    EXPECT_FLOAT_EQ(v1.x, 2.0f);
    EXPECT_FLOAT_EQ(v1.y, 4.0f);

    v1 /= 2.0f;
    EXPECT_FLOAT_EQ(v1.x, 1.0f);
    EXPECT_FLOAT_EQ(v1.y, 2.0f);
}

TEST(Vec2Tests, UtilityFunctions) {
    vec2 v1(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(v1.length(), 5.0f);
    EXPECT_FLOAT_EQ(v1.length_squared(), 25.0f);

    vec2 v2 = v1.normalized();
    EXPECT_FLOAT_EQ(v2.length(), 1.0f);

    vec2 v3(1.0f, 0.0f);
    vec2 v4(0.0f, 1.0f);
    EXPECT_FLOAT_EQ(v3.dot_product(v4), 0.0f);

    float v5 = v3.cross_product(v4);
    EXPECT_FLOAT_EQ(v5, 1.0f);
}

// Test vec3 class
TEST(Vec3Tests, Constructors) {
    vec3 v1;
    EXPECT_FLOAT_EQ(v1.x, 0.0f);
    EXPECT_FLOAT_EQ(v1.y, 0.0f);
    EXPECT_FLOAT_EQ(v1.z, 0.0f);

    vec3 v2(1.0f, 2.0f, 3.0f);
    EXPECT_FLOAT_EQ(v2.x, 1.0f);
    EXPECT_FLOAT_EQ(v2.y, 2.0f);
    EXPECT_FLOAT_EQ(v2.z, 3.0f);
}

TEST(Vec3Tests, ArithmeticOperators) {
    vec3 v1(1.0f, 2.0f, 3.0f);
    vec3 v2(4.0f, 5.0f, 6.0f);

    vec3 v3 = v1 + v2;
    EXPECT_FLOAT_EQ(v3.x, 5.0f);
    EXPECT_FLOAT_EQ(v3.y, 7.0f);
    EXPECT_FLOAT_EQ(v3.z, 9.0f);

    vec3 v4 = v1 - v2;
    EXPECT_FLOAT_EQ(v4.x, -3.0f);
    EXPECT_FLOAT_EQ(v4.y, -3.0f);
    EXPECT_FLOAT_EQ(v4.z, -3.0f);

    vec3 v5 = v1 * 2.0f;
    EXPECT_FLOAT_EQ(v5.x, 2.0f);
    EXPECT_FLOAT_EQ(v5.y, 4.0f);
    EXPECT_FLOAT_EQ(v5.z, 6.0f);

    vec3 v6 = v1 / 2.0f;
    EXPECT_FLOAT_EQ(v6.x, 0.5f);
    EXPECT_FLOAT_EQ(v6.y, 1.0f);
    EXPECT_FLOAT_EQ(v6.z, 1.5f);
}

TEST(Vec3Tests, CompoundAssignmentOperators) {
    vec3 v1(1.0f, 2.0f, 3.0f);
    vec3 v2(4.0f, 5.0f, 6.0f);

    v1 += v2;
    EXPECT_FLOAT_EQ(v1.x, 5.0f);
    EXPECT_FLOAT_EQ(v1.y, 7.0f);
    EXPECT_FLOAT_EQ(v1.z, 9.0f);

    v1 -= v2;
    EXPECT_FLOAT_EQ(v1.x, 1.0f);
    EXPECT_FLOAT_EQ(v1.y, 2.0f);
    EXPECT_FLOAT_EQ(v1.z, 3.0f);

    v1 *= 2.0f;
    EXPECT_FLOAT_EQ(v1.x, 2.0f);
    EXPECT_FLOAT_EQ(v1.y, 4.0f);
    EXPECT_FLOAT_EQ(v1.z, 6.0f);

    v1 /= 2.0f;
    EXPECT_FLOAT_EQ(v1.x, 1.0f);
    EXPECT_FLOAT_EQ(v1.y, 2.0f);
    EXPECT_FLOAT_EQ(v1.z, 3.0f);
}

TEST(Vec3Tests, UtilityFunctions) {
    vec3 v1(3.0f, 4.0f, 0.0f);
    EXPECT_FLOAT_EQ(v1.length(), 5.0f);
    EXPECT_FLOAT_EQ(v1.length_squared(), 25.0f);

    vec3 v2 = v1.normalized();
    EXPECT_FLOAT_EQ(v2.length(), 1.0f);

    vec3 v3(1.0f, 0.0f, 0.0f);
    vec3 v4(0.0f, 1.0f, 0.0f);
    EXPECT_FLOAT_EQ(v3.dot_product(v4), 0.0f);

    vec3 v5 = v3.cross_product(v4);
    EXPECT_FLOAT_EQ(v5.x, 0.0f);
    EXPECT_FLOAT_EQ(v5.y, 0.0f);
    EXPECT_FLOAT_EQ(v5.z, 1.0f);
}
// Test vec4 class
TEST(Vec4Tests, Constructors) {
    vec4 v1;
    EXPECT_FLOAT_EQ(v1.x, 0.0f);
    EXPECT_FLOAT_EQ(v1.y, 0.0f);
    EXPECT_FLOAT_EQ(v1.z, 0.0f);
    EXPECT_FLOAT_EQ(v1.w, 0.0f);

    vec4 v2(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_FLOAT_EQ(v2.x, 1.0f);
    EXPECT_FLOAT_EQ(v2.y, 2.0f);
    EXPECT_FLOAT_EQ(v2.z, 3.0f);
    EXPECT_FLOAT_EQ(v2.w, 4.0f);
}

TEST(Vec4Tests, ArithmeticOperators) {
    vec4 v1(1.0f, 2.0f, 3.0f, 4.0f);
    vec4 v2(5.0f, 6.0f, 7.0f, 8.0f);

    vec4 v3 = v1 + v2;
    EXPECT_FLOAT_EQ(v3.x, 6.0f);
    EXPECT_FLOAT_EQ(v3.y, 8.0f);
    EXPECT_FLOAT_EQ(v3.z, 10.0f);
    EXPECT_FLOAT_EQ(v3.w, 12.0f);

    vec4 v4 = v1 - v2;
    EXPECT_FLOAT_EQ(v4.x, -4.0f);
    EXPECT_FLOAT_EQ(v4.y, -4.0f);
    EXPECT_FLOAT_EQ(v4.z, -4.0f);
    EXPECT_FLOAT_EQ(v4.w, -4.0f);

    vec4 v5 = v1 * 2.0f;
    EXPECT_FLOAT_EQ(v5.x, 2.0f);
    EXPECT_FLOAT_EQ(v5.y, 4.0f);
    EXPECT_FLOAT_EQ(v5.z, 6.0f);
    EXPECT_FLOAT_EQ(v5.w, 8.0f);

    vec4 v6 = v1 / 2.0f;
    EXPECT_FLOAT_EQ(v6.x, 0.5f);
    EXPECT_FLOAT_EQ(v6.y, 1.0f);
    EXPECT_FLOAT_EQ(v6.z, 1.5f);
    EXPECT_FLOAT_EQ(v6.w, 2.0f);
}

TEST(Vec4Tests, CompoundAssignmentOperators) {
    vec4 v1(1.0f, 2.0f, 3.0f, 4.0f);
    vec4 v2(5.0f, 6.0f, 7.0f, 8.0f);

    v1 += v2;
    EXPECT_FLOAT_EQ(v1.x, 6.0f);
    EXPECT_FLOAT_EQ(v1.y, 8.0f);
    EXPECT_FLOAT_EQ(v1.z, 10.0f);
    EXPECT_FLOAT_EQ(v1.w, 12.0f);

    v1 -= v2;
    EXPECT_FLOAT_EQ(v1.x, 1.0f);
    EXPECT_FLOAT_EQ(v1.y, 2.0f);
    EXPECT_FLOAT_EQ(v1.z, 3.0f);
    EXPECT_FLOAT_EQ(v1.w, 4.0f);

    v1 *= 2.0f;
    EXPECT_FLOAT_EQ(v1.x, 2.0f);
    EXPECT_FLOAT_EQ(v1.y, 4.0f);
    EXPECT_FLOAT_EQ(v1.z, 6.0f);
    EXPECT_FLOAT_EQ(v1.w, 8.0f);

    v1 /= 2.0f;
    EXPECT_FLOAT_EQ(v1.x, 1.0f);
    EXPECT_FLOAT_EQ(v1.y, 2.0f);
    EXPECT_FLOAT_EQ(v1.z, 3.0f);
    EXPECT_FLOAT_EQ(v1.w, 4.0f);
}

TEST(Vec4Tests, UtilityFunctions) {
    vec4 v1(3.0f, 4.0f, 0.0f, 0.0f);
    EXPECT_FLOAT_EQ(v1.length(), 5.0f);
    EXPECT_FLOAT_EQ(v1.length_squared(), 25.0f);

    vec4 v2 = v1.normalized();
    EXPECT_FLOAT_EQ(v2.length(), 1.0f);

    vec4 v3(1.0f, 0.0f, 0.0f, 0.0f);
    vec4 v4(0.0f, 1.0f, 0.0f, 0.0f);
    EXPECT_FLOAT_EQ(v3.dot_product(v4), 0.0f);
}