#pragma once
class Mat {
public:
    float m[4][4] = {0};

    Mat() = default;
    Mat(float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33) {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
        m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    }

    Mat operator+(const Mat& other) const {
        return {
                m[0][0] + other.m[0][0], m[0][1] + other.m[0][1], m[0][2] + other.m[0][2], m[0][3] + other.m[0][3],
                m[1][0] + other.m[1][0], m[1][1] + other.m[1][1], m[1][2] + other.m[1][2], m[1][3] + other.m[1][3],
                m[2][0] + other.m[2][0], m[2][1] + other.m[2][1], m[2][2] + other.m[2][2], m[2][3] + other.m[2][3],
                m[3][0] + other.m[3][0], m[3][1] + other.m[3][1], m[3][2] + other.m[3][2], m[3][3] + other.m[3][3]
        };
    }

    Mat operator-(const Mat& other) const {
        return {
                m[0][0] - other.m[0][0], m[0][1] - other.m[0][1], m[0][2] - other.m[0][2], m[0][3] - other.m[0][3],
                m[1][0] - other.m[1][0], m[1][1] - other.m[1][1], m[1][2] - other.m[1][2], m[1][3] - other.m[1][3],
                m[2][0] - other.m[2][0], m[2][1] - other.m[2][1], m[2][2] - other.m[2][2], m[2][3] - other.m[2][3],
                m[3][0] - other.m[3][0], m[3][1] - other.m[3][1], m[3][2] - other.m[3][2], m[3][3] - other.m[3][3]
        };
    }

    Mat operator*(const Mat& other) const {
        Mat result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = 0;
                for (int k = 0; k < 4; ++k) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    Mat& operator+=(const Mat& other) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m[i][j] += other.m[i][j];
            }
        }
        return *this;
    }

    Mat& operator-=(const Mat& other) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m[i][j] -= other.m[i][j];
            }
        }
        return *this;
    }

    Mat& operator*=(const Mat& other) {
        *this = *this * other;
        return *this;
    }

    Mat operator*(float scalar) const {
        Mat result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = m[i][j] * scalar;
            }
        }
        return result;
    }

    Mat& operator*=(float scalar) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m[i][j] *= scalar;
            }
        }
        return *this;
    }
    bool operator==(const Mat& other) const {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (m[i][j] != other.m[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const Mat& other) const {
        return !(*this == other);
    }

    float determinant() const {
        // Calculate the determinant of the 4x4 matrix
        float det = 0.0f;
        for (int i = 0; i < 4; ++i) {
            Mat subMat;
            for (int j = 1; j < 4; ++j) {
                int subCol = 0;
                for (int k = 0; k < 4; ++k) {
                    if (k == i) continue;
                    subMat.m[j-1][subCol] = m[j][k];
                    ++subCol;
                }
            }
            det += (i % 2 == 0 ? 1 : -1) * m[0][i] * subMat.determinant3x3();
        }
        return det;
    }

    Mat transpose() const {
        Mat result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = m[j][i];
            }
        }
        return result;
    }

    Mat inverse() const {
        float det = determinant();
        assert(det != 0 && "Matrix is not invertible.");
        Mat adjugate;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                Mat subMat;
                int subRow = 0;
                for (int k = 0; k < 4; ++k) {
                    if (k == i) continue;
                    int subCol = 0;
                    for (int l = 0; l < 4; ++l) {
                        if (l == j) continue;
                        subMat.m[subRow][subCol] = m[k][l];
                        ++subCol;
                    }
                    ++subRow;
                }
                adjugate.m[j][i] = ((i + j) % 2 == 0 ? 1 : -1) * subMat.determinant3x3();
            }
        }
        return adjugate * (1.0f / det);
    }

    bool isIdentity() const {
        Mat identity = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        };
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (m[i][j] != identity.m[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

private:
    float determinant3x3() const {
        return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])
               - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
               + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    }
};