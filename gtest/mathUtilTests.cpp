#include <gtest/gtest.h>
#include "../sources/MathUtil.hpp"

TEST(MathUtilTests, Clamp) {
    EXPECT_FLOAT_EQ(MathUtil::clamp(5.0f, 0.0f, 10.0f), 5.0f);
    EXPECT_FLOAT_EQ(MathUtil::clamp(-5.0f, 0.0f, 10.0f), 0.0f);
    EXPECT_FLOAT_EQ(MathUtil::clamp(15.0f, 0.0f, 10.0f), 10.0f);
}

TEST(MathUtilTests, Lerp) {
    EXPECT_FLOAT_EQ(MathUtil::lerp(0.0f, 10.0f, 0.0f), 0.0f);
    EXPECT_FLOAT_EQ(MathUtil::lerp(0.0f, 10.0f, 0.5f), 5.0f);
    EXPECT_FLOAT_EQ(MathUtil::lerp(0.0f, 10.0f, 1.0f), 10.0f);
}

TEST(MathUtilTests, Distance) {
    EXPECT_FLOAT_EQ(MathUtil::distance(0.0f, 0.0f, 0.0f, 0.0f), 0.0f);
    EXPECT_FLOAT_EQ(MathUtil::distance(0.0f, 0.0f, 3.0f, 4.0f), 5.0f);
}

TEST(MathUtilTests, ToRadians) {
    EXPECT_FLOAT_EQ(MathUtil::to_radians(180.0f), MathUtil::pi);
    EXPECT_FLOAT_EQ(MathUtil::to_radians(360.0f), MathUtil::two_pi);
}

TEST(MathUtilTests, ToDegrees) {
    EXPECT_FLOAT_EQ(MathUtil::to_degrees(MathUtil::pi), 180.0f);
    EXPECT_FLOAT_EQ(MathUtil::to_degrees(MathUtil::two_pi), 360.0f);
}

TEST(MathUtilTests, RandomFloat) {
    float value = MathUtil::random_float(0.0f, 1.0f);
    EXPECT_GE(value, 0.0f);
    EXPECT_LE(value, 1.0f);
}

TEST(MathUtilTests, SmoothStep) {
    EXPECT_FLOAT_EQ(MathUtil::smooth_step(0.0f, 1.0f, 0.5f), 0.5f);
    EXPECT_FLOAT_EQ(MathUtil::smooth_step(0.0f, 1.0f, -0.5f), 0.0f);
    EXPECT_FLOAT_EQ(MathUtil::smooth_step(0.0f, 1.0f, 1.5f), 1.0f);
}

TEST(MathUtilTests, SmootherStep) {
    EXPECT_FLOAT_EQ(MathUtil::smoother_step(0.0f, 1.0f, 0.5f), 0.5f);
    EXPECT_FLOAT_EQ(MathUtil::smoother_step(0.0f, 1.0f, -0.5f), 0.0f);
    EXPECT_FLOAT_EQ(MathUtil::smoother_step(0.0f, 1.0f, 1.5f), 1.0f);
}

TEST(MathUtilTests, Sign) {
    EXPECT_FLOAT_EQ(MathUtil::sign(5.0f), 1.0f);
    EXPECT_FLOAT_EQ(MathUtil::sign(-5.0f), -1.0f);
    EXPECT_FLOAT_EQ(MathUtil::sign(0.0f), 0.0f);
}

TEST(MathUtilTests, Max) {
    EXPECT_FLOAT_EQ(MathUtil::max(5.0f, 10.0f), 10.0f);
    EXPECT_FLOAT_EQ(MathUtil::max(-5.0f, -10.0f), -5.0f);
}

TEST(MathUtilTests, Min) {
    EXPECT_FLOAT_EQ(MathUtil::min(5.0f, 10.0f), 5.0f);
    EXPECT_FLOAT_EQ(MathUtil::min(-5.0f, -10.0f), -10.0f);
}

TEST(MathUtilTests, Pow) {
    EXPECT_FLOAT_EQ(MathUtil::pow(2.0f, 3.0f), 8.0f);
    EXPECT_FLOAT_EQ(MathUtil::pow(4.0f, 0.5f), 2.0f);
}

TEST(MathUtilTests, Sqrt) {
    EXPECT_FLOAT_EQ(MathUtil::sqrt(4.0f), 2.0f);
    EXPECT_FLOAT_EQ(MathUtil::sqrt(9.0f), 3.0f);
}

TEST(MathUtilTests, Abs) {
    EXPECT_FLOAT_EQ(MathUtil::abs(5.0f), 5.0f);
    EXPECT_FLOAT_EQ(MathUtil::abs(-5.0f), 5.0f);
}