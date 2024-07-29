#include <gtest/gtest.h>
#include "../sources/Quaternion.hpp"

TEST(QuaternionTests, AdditionTest) {
    Quat q1(1, 2, 3, 4);
    Quat q2(4, 3, 2, 1);
    Quat result = q1 + q2;
    Quat expected(5, 5, 5, 5);
    ASSERT_EQ(result.w, expected.w);
    ASSERT_EQ(result.x, expected.x);
    ASSERT_EQ(result.y, expected.y);
    ASSERT_EQ(result.z, expected.z);
}

TEST(QuaternionTests, SubtractionTest) {
    Quat q1(1, 2, 3, 4);
    Quat q2(4, 3, 2, 1);
    Quat result = q1 - q2;
    Quat expected(-3, -1, 1, 3);
    ASSERT_EQ(result.w, expected.w);
    ASSERT_EQ(result.x, expected.x);
    ASSERT_EQ(result.y, expected.y);
    ASSERT_EQ(result.z, expected.z);
}

TEST(QuaternionTests, MultiplicationTest) {
    Quat q1(1, 2, 3, 4);
    Quat q2(4, 3, 2, 1);
    Quat result = q1 * q2;
    Quat expected(-12, 6, 24, 12);
    ASSERT_EQ(result.w, expected.w);
    ASSERT_EQ(result.x, expected.x);
    ASSERT_EQ(result.y, expected.y);
    ASSERT_EQ(result.z, expected.z);
}

TEST(QuaternionTests, ScalarMultiplicationTest) {
    Quat q(1, 2, 3, 4);
    Quat result = q * 2.0f;
    Quat expected(2, 4, 6, 8);
    ASSERT_EQ(result.w, expected.w);
    ASSERT_EQ(result.x, expected.x);
    ASSERT_EQ(result.y, expected.y);
    ASSERT_EQ(result.z, expected.z);
}

TEST(QuaternionTests, NormTest) {
    Quat q(1, 2, 3, 4);
    float result = q.norm();
    float expected = std::sqrt(30);
    ASSERT_FLOAT_EQ(result, expected);
}

TEST(QuaternionTests, NormalizationTest) {
    Quat q(1, 2, 3, 4);
    Quat result = q.normalized();
    float norm = std::sqrt(30);
    Quat expected(1 / norm, 2 / norm, 3 / norm, 4 / norm);
    ASSERT_FLOAT_EQ(result.w, expected.w);
    ASSERT_FLOAT_EQ(result.x, expected.x);
    ASSERT_FLOAT_EQ(result.y, expected.y);
    ASSERT_FLOAT_EQ(result.z, expected.z);
}

TEST(QuaternionTests, ConjugateTest) {
    Quat q(1, 2, 3, 4);
    Quat result = q.conjugate();
    Quat expected(1, -2, -3, -4);
    ASSERT_EQ(result.w, expected.w);
    ASSERT_EQ(result.x, expected.x);
    ASSERT_EQ(result.y, expected.y);
    ASSERT_EQ(result.z, expected.z);
}

TEST(QuaternionTests, InverseTest) {
    Quat q(1, 2, 3, 4);
    Quat result = q.inverse();
    float norm = std::sqrt(30);
    Quat expected(1 / (norm * norm), -2 / (norm * norm), -3 / (norm * norm), -4 / (norm * norm));
    ASSERT_FLOAT_EQ(result.w, expected.w);
    ASSERT_FLOAT_EQ(result.x, expected.x);
    ASSERT_FLOAT_EQ(result.y, expected.y);
    ASSERT_FLOAT_EQ(result.z, expected.z);
}