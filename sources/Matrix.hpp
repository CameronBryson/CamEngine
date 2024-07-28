#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cmath>

class Mat2 {
public:
    float m[2][2] = {};

    Mat2() = default;
    Mat2(float a, float b, float c, float d) : m{{a, b}, {c, d}} {}

    Mat2 operator+(const Mat2& other) const {
        return {{m[0][0] + other.m[0][0], m[0][1] + other.m[0][1]},
                {m[1][0] + other.m[1][0], m[1][1] + other.m[1][1]}};
    }

    Mat2 operator-(const Mat2& other) const {
        return {{m[0][0] - other.m[0][0], m[0][1] - other.m[0][1]},
                {m[1][0] - other.m[1][0], m[1][1] - other.m[1][1]}};
    }

    Mat2 operator*(const Mat2& other) const {
        return {{m[0][0] * other.m[0][0] + m[0][1] * other.m[1][0], m[0][0] * other.m[0][1] + m[0][1] * other.m[1][1]},
                {m[1][0] * other.m[0][0] + m[1][1] * other.m[1][0], m[1][0] * other.m[0][1] + m[1][1] * other.m[1][1]}};
    }

    Mat2& operator+=(const Mat2& other) {
        m[0][0] += other.m[0][0]; m[0][1] += other.m[0][1];
        m[1][0] += other.m[1][0]; m[1][1] += other.m[1][1];
        return *this;
    }

    Mat2& operator-=(const Mat2& other) {
        m[0][0] -= other.m[0][0]; m[0][1] -= other.m[0][1];
        m[1][0] -= other.m[1][0]; m[1][1] -= other.m[1][1];
        return *this;
    }

    Mat2& operator*=(const Mat2& other) {
        *this = *this * other;
        return *this;
    }
};

class Mat3 {
public:
    float m[3][3] = {};

    Mat3() = default;
    Mat3(float a, float b, float c, float d, float e, float f, float g, float h, float i)
        : m{{a, b, c}, {d, e, f}, {g, h, i}} {}

    Mat3 operator+(const Mat3& other) const {
        return {{m[0][0] + other.m[0][0], m[0][1] + other.m[0][1], m[0][2] + other.m[0][2]},
                {m[1][0] + other.m[1][0], m[1][1] + other.m[1][1], m[1][2] + other.m[1][2]},
                {m[2][0] + other.m[2][0], m[2][1] + other.m[2][1], m[2][2] + other.m[2][2]}};
    }

    Mat3 operator-(const Mat3& other) const {
        return {{m[0][0] - other.m[0][0], m[0][1] - other.m[0][1], m[0][2] - other.m[0][2]},
                {m[1][0] - other.m[1][0], m[1][1] - other.m[1][1], m[1][2] - other.m[1][2]},
                {m[2][0] - other.m[2][0], m[2][1] - other.m[2][1], m[2][2] - other.m[2][2]}};
    }

    Mat3 operator*(const Mat3& other) const {
        return {{m[0][0] * other.m[0][0] + m[0][1] * other.m[1][0] + m[0][2] * other.m[2][0],
                 m[0][0] * other.m[0][1] + m[0][1] * other.m[1][1] + m[0][2] * other.m[2][1],
                 m[0][0] * other.m[0][2] + m[0][1] * other.m[1][2] + m[0][2] * other.m[2][2]},
                {m[1][0] * other.m[0][0] + m[1][1] * other.m[1][0] + m[1][2] * other.m[2][0],
                 m[1][0] * other.m[0][1] + m[1][1] * other.m[1][1] + m[1][2] * other.m[2][1],
                 m[1][0] * other.m[0][2] + m[1][1] * other.m[1][2] + m[1][2] * other.m[2][2]},
                {m[2][0] * other.m[0][0] + m[2][1] * other.m[1][0] + m[2][2] * other.m[2][0],
                 m[2][0] * other.m[0][1] + m[2][1] * other.m[1][1] + m[2][2] * other.m[2][1],
                 m[2][0] * other.m[0][2] + m[2][1] * other.m[1][2] + m[2][2] * other.m[2][2]}};
    }

    Mat3& operator+=(const Mat3& other) {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                m[i][j] += other.m[i][j];
        return *this;
    }

    Mat3& operator-=(const Mat3& other) {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                m[i][j] -= other.m[i][j];
        return *this;
    }

    Mat3& operator*=(const Mat3& other) {
        *this = *this * other;
        return *this;
    }
};

class Mat4 {
public:
    float m[4][4] = {};

    Mat4() = default;
    Mat4(float a, float b, float c, float d, float e, float f, float g, float h,
              float i, float j, float k, float l, float m, float n, float o, float p)
        : m{{a, b, c, d}, {e, f, g, h}, {i, j, k, l}, {m, n, o, p}} {}

    Mat4 operator+(const Mat4& other) const {
        return {{m[0][0] + other.m[0][0], m[0][1] + other.m[0][1], m[0][2] + other.m[0][2], m[0][3] + other.m[0][3]},
                {m[1][0] + other.m[1][0], m[1][1] + other.m[1][1], m[1][2] + other.m[1][2], m[1][3] + other.m[1][3]},
                {m[2][0] + other.m[2][0], m[2][1] + other.m[2][1], m[2][2] + other.m[2][2], m[2][3] + other.m[2][3]},
                {m[3][0] + other.m[3][0], m[3][1] + other.m[3][1], m[3][2] + other.m[3][2], m[3][3] + other.m[3][3]}};
    }

    Mat4 operator-(const Mat4& other) const {
        return {{m[0][0] - other.m[0][0], m[0][1] - other.m[0][1], m[0][2] - other.m[0][2], m[0][3] - other.m[0][3]},
                {m[1][0] - other.m[1][0], m[1][1] - other.m[1][1], m[1][2] - other.m[1][2], m[1][3] - other.m[1][3]},
                {m[2][0] - other.m[2][0], m[2][1] - other.m[2][1], m[2][2] - other.m[2][2], m[2][3] - other.m[2][3]},
                {m[3][0] - other.m[3][0], m[3][1] - other.m[3][1], m[3][2] - other.m[3][2], m[3][3] - other.m[3][3]}};
    }

    Mat4 operator*(const Mat4& other) const {
        Mat4 result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                for (int k = 0; k < 4; ++k)
                    result.m[i][j] += m[i][k] * other.m[k][j];
        return result;
    }

    Mat4& operator+=(const Mat4& other) {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] += other.m[i][j];
        return *this;
    }

    Mat4& operator-=(const Mat4& other) {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] -= other.m[i][j];
        return *this;
    }

    Mat4& operator*=(const Mat4& other) {
        *this = *this * other;
        return *this;
    }
};

#endif // MATRIX_HPP