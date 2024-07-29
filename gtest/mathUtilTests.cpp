#include "../sources/MathUtil.hpp"
#include <gtest/gtest.h>

class math_util_test : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Setup code if needed
    }

    void TearDown() override
    {
        // Cleanup code if needed
    }
};

TEST_F(math_util_test, ClampWithinRange)
{
    EXPECT_FLOAT_EQ(MathUtil::clamp(5.0f, 0.0f, 10.0f), 5.0f);
}

TEST_F(math_util_test, ClampBelowRange)
{
    EXPECT_FLOAT_EQ(MathUtil::clamp(-5.0f, 0.0f, 10.0f), 0.0f);
}

TEST_F(math_util_test, ClampAboveRange)
{
    EXPECT_FLOAT_EQ(MathUtil::clamp(15.0f, 0.0f, 10.0f), 10.0f);
}

TEST_F(math_util_test, Lerp)
{
    EXPECT_FLOAT_EQ(MathUtil::lerp(0.0f, 10.0f, 0.5f), 5.0f);
}

TEST_F(math_util_test, Distance)
{
    EXPECT_FLOAT_EQ(MathUtil::distance(0.0f, 0.0f, 3.0f, 4.0f), 5.0f);
}

TEST_F(math_util_test, ToRadians)
{
    EXPECT_FLOAT_EQ(MathUtil::to_radians(180.0f), MathUtil::pi);
}

TEST_F(math_util_test, ToDegrees)
{
    EXPECT_FLOAT_EQ(MathUtil::to_degrees(MathUtil::pi), 180.0f);
}

TEST_F(math_util_test, RandomFloat)
{
    float value = MathUtil::random_float(0.0f, 1.0f);
    EXPECT_GE(value, 0.0f);
    EXPECT_LE(value, 1.0f);
}

TEST_F(math_util_test, SmoothStep)
{
    EXPECT_FLOAT_EQ(MathUtil::smooth_step(0.0f, 1.0f, 0.5f), 0.5f);
}

TEST_F(math_util_test, SmootherStep)
{
    EXPECT_FLOAT_EQ(MathUtil::smoother_step(0.0f, 1.0f, 0.5f), 0.5f);
}

TEST_F(math_util_test, SignPositive)
{
    EXPECT_FLOAT_EQ(MathUtil::sign(5.0f), 1.0f);
}

TEST_F(math_util_test, SignNegative)
{
    EXPECT_FLOAT_EQ(MathUtil::sign(-5.0f), -1.0f);
}

TEST_F(math_util_test, SignZero)
{
    EXPECT_FLOAT_EQ(MathUtil::sign(0.0f), 0.0f);
}

TEST_F(math_util_test, Max)
{
    EXPECT_FLOAT_EQ(MathUtil::max(5.0f, 10.0f), 10.0f);
}

TEST_F(math_util_test, Min)
{
    EXPECT_FLOAT_EQ(MathUtil::min(5.0f, 10.0f), 5.0f);
}

TEST_F(math_util_test, Pow)
{
    EXPECT_FLOAT_EQ(MathUtil::pow(2.0f, 3.0f), 8.0f);
}

TEST_F(math_util_test, Sqrt)
{
    EXPECT_FLOAT_EQ(MathUtil::sqrt(9.0f), 3.0f);
}

TEST_F(math_util_test, AbsPositive)
{
    EXPECT_FLOAT_EQ(MathUtil::abs(5.0f), 5.0f);
}

TEST_F(math_util_test, AbsNegative)
{
    EXPECT_FLOAT_EQ(MathUtil::abs(-5.0f), 5.0f);
}
