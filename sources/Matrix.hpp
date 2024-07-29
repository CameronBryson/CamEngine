#pragma once
#include <cassert>
class Mat
{
  public:
    float m[4][4] = {0};

    Mat() = default;
    Mat(const float m00, const float m01, const float m02, const float m03, const float m10, const float m11,
        const float m12, const float m13, const float m20, const float m21, const float m22, const float m23,
        const float m30, const float m31, const float m32, const float m33)
    {
        m[0][0] = m00;
        m[0][1] = m01;
        m[0][2] = m02;
        m[0][3] = m03;
        m[1][0] = m10;
        m[1][1] = m11;
        m[1][2] = m12;
        m[1][3] = m13;
        m[2][0] = m20;
        m[2][1] = m21;
        m[2][2] = m22;
        m[2][3] = m23;
        m[3][0] = m30;
        m[3][1] = m31;
        m[3][2] = m32;
        m[3][3] = m33;
    }

    Mat operator+(const Mat &other) const
    {
        return {m[0][0] + other.m[0][0], m[0][1] + other.m[0][1], m[0][2] + other.m[0][2], m[0][3] + other.m[0][3],
                m[1][0] + other.m[1][0], m[1][1] + other.m[1][1], m[1][2] + other.m[1][2], m[1][3] + other.m[1][3],
                m[2][0] + other.m[2][0], m[2][1] + other.m[2][1], m[2][2] + other.m[2][2], m[2][3] + other.m[2][3],
                m[3][0] + other.m[3][0], m[3][1] + other.m[3][1], m[3][2] + other.m[3][2], m[3][3] + other.m[3][3]};
    }

    Mat operator-(const Mat &other) const
    {
        return {m[0][0] - other.m[0][0], m[0][1] - other.m[0][1], m[0][2] - other.m[0][2], m[0][3] - other.m[0][3],
                m[1][0] - other.m[1][0], m[1][1] - other.m[1][1], m[1][2] - other.m[1][2], m[1][3] - other.m[1][3],
                m[2][0] - other.m[2][0], m[2][1] - other.m[2][1], m[2][2] - other.m[2][2], m[2][3] - other.m[2][3],
                m[3][0] - other.m[3][0], m[3][1] - other.m[3][1], m[3][2] - other.m[3][2], m[3][3] - other.m[3][3]};
    }

    Mat operator*(const Mat &other) const
    {
        Mat result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result.m[i][j] = 0;
                for (int k = 0; k < 4; ++k)
                {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    Mat &operator+=(const Mat &other)
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                m[i][j] += other.m[i][j];
            }
        }
        return *this;
    }

    Mat &operator-=(const Mat &other)
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                m[i][j] -= other.m[i][j];
            }
        }
        return *this;
    }

    Mat &operator*=(const Mat &other)
    {
        *this = *this * other;
        return *this;
    }

    Mat operator*(const float scalar) const
    {
        Mat result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result.m[i][j] = m[i][j] * scalar;
            }
        }
        return result;
    }

    Mat &operator*=(const float scalar)
    {
        for (auto &i : m)
        {
            for (float &j : i)
            {
                j *= scalar;
            }
        }
        return *this;
    }
    bool operator==(const Mat &other) const
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (m[i][j] != other.m[i][j])
                {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const Mat &other) const
    {
        return !(*this == other);
    }

    [[nodiscard]] float determinant() const
    {
        // Calculate the determinant of the 4x4 matrix
        float det = 0.0f;
        for (int i = 0; i < 4; ++i)
        {
            Mat sub_mat;
            for (int j = 1; j < 4; ++j)
            {
                int sub_col = 0;
                for (int k = 0; k < 4; ++k)
                {
                    if (k == i)
                        continue;
                    sub_mat.m[j - 1][sub_col] = m[j][k];
                    ++sub_col;
                }
            }
            det += (i % 2 == 0 ? 1 : -1) * m[0][i] * sub_mat.determinant3_x3();
        }
        return det;
    }

    [[nodiscard]] Mat transpose() const
    {
        Mat result;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result.m[i][j] = m[j][i];
            }
        }
        return result;
    }

    [[nodiscard]] Mat inverse() const
    {
        const float det = determinant();
        assert(det != 0 && "Matrix is not invertible.");
        Mat adjugate;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                Mat sub_mat;
                int sub_row = 0;
                for (int k = 0; k < 4; ++k)
                {
                    if (k == i)
                        continue;
                    int sub_col = 0;
                    for (int l = 0; l < 4; ++l)
                    {
                        if (l == j)
                            continue;
                        sub_mat.m[sub_row][sub_col] = m[k][l];
                        ++sub_col;
                    }
                    ++sub_row;
                }
                adjugate.m[j][i] = ((i + j) % 2 == 0 ? 1 : -1) * sub_mat.determinant3_x3();
            }
        }
        return adjugate * (1.0f / det);
    }

    [[nodiscard]] bool is_identity() const
    {
        const Mat identity = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (m[i][j] != identity.m[i][j])
                {
                    return false;
                }
            }
        }
        return true;
    }

  private:
    [[nodiscard]] float determinant3_x3() const
    {
        return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
               m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    }
};