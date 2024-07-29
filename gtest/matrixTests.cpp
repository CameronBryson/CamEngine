#include <gtest/gtest.h>
#include "../sources/Matrix.hpp"

TEST(MatrixTests, AdditionTest) {
    Mat mat1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    Mat mat2(16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
    Mat result = mat1 + mat2;
    Mat expected(17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17);
    ASSERT_EQ(result, expected);
}

TEST(MatrixTests, SubtractionTest) {
    Mat mat1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    Mat mat2(16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
    Mat result = mat1 - mat2;
    Mat expected(-15, -13, -11, -9, -7, -5, -3, -1, 1, 3, 5, 7, 9, 11, 13, 15);
    ASSERT_EQ(result, expected);
}

TEST(MatrixTests, MultiplicationTest) {
    Mat mat1(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    Mat mat2(16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
    Mat result = mat1 * mat2;
    Mat expected(80, 70, 60, 50, 240, 214, 188, 162, 400, 358, 316, 274, 560, 502, 444, 386);
    ASSERT_EQ(result, expected);
}

TEST(MatrixTests, ScalarMultiplicationTest) {
    Mat mat(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    Mat result = mat * 2.0f;
    Mat expected(2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32);
    ASSERT_EQ(result, expected);
}

TEST(MatrixTests, DeterminantTest) {
    Mat mat(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    float det = mat.determinant();
    ASSERT_EQ(det, 0.0f);
}

TEST(MatrixTests, TransposeTest) {
    Mat mat(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    Mat result = mat.transpose();
    Mat expected(1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16);
    ASSERT_EQ(result, expected);
}

TEST(MatrixTests, InverseTest) {
    Mat mat(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    Mat result = mat.inverse();
    Mat expected(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    ASSERT_EQ(result, expected);
}

TEST(MatrixTests, IdentityTest) {
    Mat mat(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    ASSERT_TRUE(mat.isIdentity());
}