#include "../sources/Matrix.hpp"
#include <gtest/gtest.h>

TEST(MatrixTests, AdditionTest)
{
    mat4 mat1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    mat4 mat2(16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
    mat4 result = mat1 + mat2;
    mat4 expected(17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17);
    ASSERT_EQ(result, expected);
}

TEST(MatrixTests, SubtractionTest)
{
    mat4 mat1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    mat4 mat2(16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
    mat4 result = mat1 - mat2;
    mat4 expected(-15, -13, -11, -9, -7, -5, -3, -1, 1, 3, 5, 7, 9, 11, 13, 15);
    ASSERT_EQ(result, expected);
}

TEST(MatrixTests, MultiplicationTest)
{
    mat4 mat1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    mat4 mat2(16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
    mat4 result = mat1 * mat2;
    mat4 expected(80, 70, 60, 50, 240, 214, 188, 162, 400, 358, 316, 274, 560, 502, 444, 386);
    ASSERT_EQ(result, expected);
}

TEST(MatrixTests, ScalarMultiplicationTest)
{
    mat4 mat(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    mat result = mat * 2.0f;
    mat expected(2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32);
    ASSERT_EQ(result, expected);
}

TEST(MatrixTests, DeterminantTest)
{
    mat4 mat(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    float det = mat.determinant();
    ASSERT_EQ(det, 0.0f);
}

TEST(MatrixTests, TransposeTest)
{
    mat4 mat(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    mat result = mat.transpose();
    mat expected(1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16);
    ASSERT_EQ(result, expected);
}

TEST(MatrixTests, InverseTest)
{
    mat4 mat(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    mat result = mat.inverse();
    mat expected(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    ASSERT_EQ(result, expected);
}

TEST(MatrixTests, IdentityTest)
{
    mat4 mat(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    ASSERT_TRUE(mat.is_identity());
}