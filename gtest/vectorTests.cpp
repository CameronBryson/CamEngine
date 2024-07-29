#include "../sources/Vectors.hpp"
#include <gtest/gtest.h>

// Vec2 Tests
TEST(Vec2Tests, DefaultConstructor)
{
    vec2 v;
    ASSERT_EQ(v.x, 0);
    ASSERT_EQ(v.y, 0);
}

TEST(Vec2Tests, ParameterizedConstructor)
{
    vec2 v(1.0f, 2.0f);
    ASSERT_EQ(v.x, 1.0f);
    ASSERT_EQ(v.y, 2.0f);
}

TEST(Vec2Tests, AdditionOperator)
{
    vec2 v1(1.0f, 2.0f);
    vec2 v2(3.0f, 4.0f);
    vec2 result = v1 + v2;
    ASSERT_EQ(result.x, 4.0f);
    ASSERT_EQ(result.y, 6.0f);
}

TEST(Vec2Tests, SubtractionOperator)
{
    vec2 v1(1.0f, 2.0f);
    vec2 v2(3.0f, 4.0f);
    vec2 result = v1 - v2;
    ASSERT_EQ(result.x, -2.0f);
    ASSERT_EQ(result.y, -2.0f);
}

TEST(Vec2Tests, MultiplicationOperator)
{
    vec2 v(1.0f, 2.0f);
    vec2 result = v * 2.0f;
    ASSERT_EQ(result.x, 2.0f);
    ASSERT_EQ(result.y, 4.0f);
}

TEST(Vec2Tests, CompoundAdditionOperator)
{
    vec2 v1(1.0f, 2.0f);
    vec2 v2(3.0f, 4.0f);
    v1 += v2;
    ASSERT_EQ(v1.x, 4.0f);
    ASSERT_EQ(v1.y, 6.0f);
}

TEST(Vec2Tests, CompoundSubtractionOperator)
{
    vec2 v1(1.0f, 2.0f);
    vec2 v2(3.0f, 4.0f);
    v1 -= v2;
    ASSERT_EQ(v1.x, -2.0f);
    ASSERT_EQ(v1.y, -2.0f);
}

TEST(Vec2Tests, CompoundMultiplicationOperator)
{
    vec2 v(1.0f, 2.0f);
    v *= 2.0f;
    ASSERT_EQ(v.x, 2.0f);
    ASSERT_EQ(v.y, 4.0f);
}

TEST(Vec2Tests, Length)
{
    vec2 v(3.0f, 4.0f);
    ASSERT_EQ(v.length(), 5.0f);
}

TEST(Vec2Tests, LengthSquared)
{
    vec2 v(3.0f, 4.0f);
    ASSERT_EQ(v.length_squared(), 25.0f);
}

TEST(Vec2Tests, Normalized)
{
    vec2 v(3.0f, 4.0f);
    vec2 result = v.normalized();
    ASSERT_NEAR(result.x, 0.6f, 1e-5);
    ASSERT_NEAR(result.y, 0.8f, 1e-5);
}

// Vec3 Tests
TEST(Vec3Tests, DefaultConstructor)
{
    vec3 v;
    ASSERT_EQ(v.x, 0);
    ASSERT_EQ(v.y, 0);
    ASSERT_EQ(v.z, 0);
}

TEST(Vec3Tests, ParameterizedConstructor)
{
    vec3 v(1.0f, 2.0f, 3.0f);
    ASSERT_EQ(v.x, 1.0f);
    ASSERT_EQ(v.y, 2.0f);
    ASSERT_EQ(v.z, 3.0f);
}

TEST(Vec3Tests, AdditionOperator)
{
    vec3 v1(1.0f, 2.0f, 3.0f);
    vec3 v2(4.0f, 5.0f, 6.0f);
    vec3 result = v1 + v2;
    ASSERT_EQ(result.x, 5.0f);
    ASSERT_EQ(result.y, 7.0f);
    ASSERT_EQ(result.z, 9.0f);
}

TEST(Vec3Tests, SubtractionOperator)
{
    vec3 v1(1.0f, 2.0f, 3.0f);
    vec3 v2(4.0f, 5.0f, 6.0f);
    vec3 result = v1 - v2;
    ASSERT_EQ(result.x, -3.0f);
    ASSERT_EQ(result.y, -3.0f);
    ASSERT_EQ(result.z, -3.0f);
}

TEST(Vec3Tests, MultiplicationOperator)
{
    vec3 v(1.0f, 2.0f, 3.0f);
    vec3 result = v * 2.0f;
    ASSERT_EQ(result.x, 2.0f);
    ASSERT_EQ(result.y, 4.0f);
    ASSERT_EQ(result.z, 6.0f);
}

TEST(Vec3Tests, CompoundAdditionOperator)
{
    vec3 v1(1.0f, 2.0f, 3.0f);
    vec3 v2(4.0f, 5.0f, 6.0f);
    v1 += v2;
    ASSERT_EQ(v1.x, 5.0f);
    ASSERT_EQ(v1.y, 7.0f);
    ASSERT_EQ(v1.z, 9.0f);
}

TEST(Vec3Tests, CompoundSubtractionOperator)
{
    vec3 v1(1.0f, 2.0f, 3.0f);
    vec3 v2(4.0f, 5.0f, 6.0f);
    v1 -= v2;
    ASSERT_EQ(v1.x, -3.0f);
    ASSERT_EQ(v1.y, -3.0f);
    ASSERT_EQ(v1.z, -3.0f);
}

TEST(Vec3Tests, CompoundMultiplicationOperator)
{
    vec3 v(1.0f, 2.0f, 3.0f);
    v *= 2.0f;
    ASSERT_EQ(v.x, 2.0f);
    ASSERT_EQ(v.y, 4.0f);
    ASSERT_EQ(v.z, 6.0f);
}

TEST(Vec3Tests, Length)
{
    vec3 v(1.0f, 2.0f, 2.0f);
    ASSERT_EQ(v.length(), 3.0f);
}

TEST(Vec3Tests, LengthSquared)
{
    vec3 v(1.0f, 2.0f, 2.0f);
    ASSERT_EQ(v.length_squared(), 9.0f);
}

TEST(Vec3Tests, Normalized)
{
    vec3 v(1.0f, 2.0f, 2.0f);
    vec3 result = v.normalized();
    ASSERT_NEAR(result.x, 1.0f / 3.0f, 1e-5);
    ASSERT_NEAR(result.y, 2.0f / 3.0f, 1e-5);
    ASSERT_NEAR(result.z, 2.0f / 3.0f, 1e-5);
}

// Vec4 Tests
TEST(Vec4Tests, DefaultConstructor)
{
    vec4 v;
    ASSERT_EQ(v.x, 0);
    ASSERT_EQ(v.y, 0);
    ASSERT_EQ(v.z, 0);
    ASSERT_EQ(v.w, 0);
}

TEST(Vec4Tests, ParameterizedConstructor)
{
    vec4 v(1.0f, 2.0f, 3.0f, 4.0f);
    ASSERT_EQ(v.x, 1.0f);
    ASSERT_EQ(v.y, 2.0f);
    ASSERT_EQ(v.z, 3.0f);
    ASSERT_EQ(v.w, 4.0f);
}

TEST(Vec4Tests, AdditionOperator)
{
    vec4 v1(1.0f, 2.0f, 3.0f, 4.0f);
    vec4 v2(5.0f, 6.0f, 7.0f, 8.0f);
    vec4 result = v1 + v2;
    ASSERT_EQ(result.x, 6.0f);
    ASSERT_EQ(result.y, 8.0f);
    ASSERT_EQ(result.z, 10.0f);
    ASSERT_EQ(result.w, 12.0f);
}

TEST(Vec4Tests, SubtractionOperator)
{
    vec4 v1(1.0f, 2.0f, 3.0f, 4.0f);
    vec4 v2(5.0f, 6.0f, 7.0f, 8.0f);
    vec4 result = v1 - v2;
    ASSERT_EQ(result.x, -4.0f);
    ASSERT_EQ(result.y, -4.0f);
    ASSERT_EQ(result.z, -4.0f);
    ASSERT_EQ(result.w, -4.0f);
}

TEST(Vec4Tests, MultiplicationOperator)
{
    vec4 v(1.0f, 2.0f, 3.0f, 4.0f);
    vec4 result = v * 2.0f;
    ASSERT_EQ(result.x, 2.0f);
    ASSERT_EQ(result.y, 4.0f);
    ASSERT_EQ(result.z, 6.0f);
    ASSERT_EQ(result.w, 8.0f);
}

TEST(Vec4Tests, CompoundAdditionOperator)
{
    vec4 v1(1.0f, 2.0f, 3.0f, 4.0f);
    vec4 v2(5.0f, 6.0f, 7.0f, 8.0f);
    v1 += v2;
    ASSERT_EQ(v1.x, 6.0f);
    ASSERT_EQ(v1.y, 8.0f);
    ASSERT_EQ(v1.z, 10.0f);
    ASSERT_EQ(v1.w, 12.0f);
}