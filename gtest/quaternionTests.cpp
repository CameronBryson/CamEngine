#include "../sources/Quaternion.hpp"
#include <gtest/gtest.h>

TEST(QuaternionTests, AdditionTest)
{
    quat q1(1, 2, 3, 4);
    quat q2(4, 3, 2, 1);
    quat result = q1 + q2;
    quat expected(5, 5, 5, 5);
    ASSERT_EQ(result.w, expected.w);
    ASSERT_EQ(result.x, expected.x);
    ASSERT_EQ(result.y, expected.y);
    ASSERT_EQ(result.z, expected.z);
}

TEST(QuaternionTests, SubtractionTest)
{
    quat q1(1, 2, 3, 4);
    quat q2(4, 3, 2, 1);
    quat result = q1 - q2;
    quat expected(-3, -1, 1, 3);
    ASSERT_EQ(result.w, expected.w);
    ASSERT_EQ(result.x, expected.x);
    ASSERT_EQ(result.y, expected.y);
    ASSERT_EQ(result.z, expected.z);
}

TEST(QuaternionTests, MultiplicationTest)
{
    quat q1(1, 2, 3, 4);
    quat q2(4, 3, 2, 1);
    quat result = q1 * q2;
    quat expected(-12, 6, 24, 12);
    ASSERT_EQ(result.w, expected.w);
    ASSERT_EQ(result.x, expected.x);
    ASSERT_EQ(result.y, expected.y);
    ASSERT_EQ(result.z, expected.z);
}

TEST(QuaternionTests, ScalarMultiplicationTest)
{
    quat q(1, 2, 3, 4);
    quat result = q * 2.0f;
    quat expected(2, 4, 6, 8);
    ASSERT_EQ(result.w, expected.w);
    ASSERT_EQ(result.x, expected.x);
    ASSERT_EQ(result.y, expected.y);
    ASSERT_EQ(result.z, expected.z);
}

TEST(QuaternionTests, NormTest)
{
    quat q(1, 2, 3, 4);
    float result = q.norm();
    float expected = std::sqrt(30);
    ASSERT_FLOAT_EQ(result, expected);
}

TEST(QuaternionTests, NormalizationTest)
{
    quat q(1, 2, 3, 4);
    quat result = q.normalized();
    float norm = std::sqrt(30);
    quat expected(1 / norm, 2 / norm, 3 / norm, 4 / norm);
    ASSERT_FLOAT_EQ(result.w, expected.w);
    ASSERT_FLOAT_EQ(result.x, expected.x);
    ASSERT_FLOAT_EQ(result.y, expected.y);
    ASSERT_FLOAT_EQ(result.z, expected.z);
}

TEST(QuaternionTests, ConjugateTest)
{
    quat q(1, 2, 3, 4);
    quat result = q.conjugate();
    quat expected(1, -2, -3, -4);
    ASSERT_EQ(result.w, expected.w);
    ASSERT_EQ(result.x, expected.x);
    ASSERT_EQ(result.y, expected.y);
    ASSERT_EQ(result.z, expected.z);
}

TEST(QuaternionTests, InverseTest)
{
    quat q(1, 2, 3, 4);
    quat result = q.inverse();
    float norm = std::sqrt(30);
    quat expected(1 / (norm * norm), -2 / (norm * norm), -3 / (norm * norm), -4 / (norm * norm));
    ASSERT_FLOAT_EQ(result.w, expected.w);
    ASSERT_FLOAT_EQ(result.x, expected.x);
    ASSERT_FLOAT_EQ(result.y, expected.y);
    ASSERT_FLOAT_EQ(result.z, expected.z);
}