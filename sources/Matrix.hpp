#pragma once
#include "Vectors.hpp"

class mat
{
public:
    float m[4][4] = {};

    mat() = default;
    mat(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33) {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
        m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
    }
    static mat create_scale_matrix(const vec3& scale) {
        return mat(
            scale.x, 0, 0, 0,
            0, scale.y, 0, 0,
            0, 0, scale.z, 0,
            0, 0, 0, 1
        );
    }
    static mat create_translation_matrix(const vec3& position)
    {
        return mat(
            1, 0, 0, position.x,
            0, 1, 0, position.y,
            0, 0, 1, position.z,
            0, 0, 0, 1
        );
    }
    static mat create_rotation_matrix(const vec3& rotation) {
        // Rotation around the X-axis
        const mat rotation_x(
            1, 0, 0, 0,
            0, cos(rotation.x), -sin(rotation.x), 0,
            0, sin(rotation.x), cos(rotation.x), 0,
            0, 0, 0, 1
        );

        // Rotation around the Y-axis
        const mat rotation_y(
            cos(rotation.y), 0, sin(rotation.y), 0,
            0, 1, 0, 0,
            -sin(rotation.y), 0, cos(rotation.y), 0,
            0, 0, 0, 1
        );

        // Rotation around the Z-axis
        const mat rotation_z(
            cos(rotation.z), -sin(rotation.z), 0, 0,
            sin(rotation.z), cos(rotation.z), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );

        // Combine the rotation matrices
        return rotation_z * rotation_y * rotation_x;
    }
    static mat create_view_matrix(const vec3& position, const vec3& target, const vec3& up) {
        vec3 z_axis = (position - target).normalized();
        vec3 x_axis = MathUtil::cross_product(up.normalized(), z_axis).normalized();
        vec3 y_axis = MathUtil::cross_product(z_axis, x_axis);

        mat translation = create_translation_matrix(position*-1.0f);
        mat rotation = mat(
            x_axis.x, x_axis.y, x_axis.z, 0,
            y_axis.x, y_axis.y, y_axis.z, 0,
            z_axis.x, z_axis.y, z_axis.z, 0,
            0, 0, 0, 1
        );
        return rotation * translation;
    }
    static mat create_perspective_matrix(const float fov, const float aspect_ratio, const float near_plane, const float far_plane) {
        const float f = 1.0f / tan(MathUtil::deg_to_rad*(fov * 0.5f));

        return mat(
            f / aspect_ratio, 0, 0, 0,
            0, -f, 0, 0,
            0, 0, far_plane/(near_plane - far_plane), -1,
            0, 0, (near_plane * far_plane) / (near_plane - far_plane), 0
        );
    }
    mat operator+(const mat &other) const {
        mat result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result.m[i][j] = m[i][j] + other.m[i][j];
        return result;
    }

    mat operator-(const mat &other) const {
        mat result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result.m[i][j] = m[i][j] - other.m[i][j];
        return result;
    }

    mat operator*(const mat &other) const {
        mat result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = 0;
                for (int k = 0; k < 4; ++k)
                    result.m[i][j] += m[i][k] * other.m[k][j];
            }
        return result;
    }

    mat operator*(const float scalar) const {
        mat result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                result.m[i][j] = m[i][j] * scalar;
        return result;
    }
    vec3 operator*(const vec3 vector) const {
        return vec3(
            m[0][0] * vector.x + m[0][1] * vector.y + m[0][2] * vector.z + m[0][3] * 1.0f,
            m[1][0] * vector.x + m[1][1] * vector.y + m[1][2] * vector.z + m[1][3] * 1.0f,
            m[2][0] * vector.x + m[2][1] * vector.y + m[2][2] * vector.z + m[2][3] * 1.0f
        );
    }
    vec4 operator*(const vec4 vector) const {
        return vec4(
            m[0][0] * vector.x + m[0][1] * vector.y + m[0][2] * vector.z + m[0][3] * vector.w,
            m[1][0] * vector.x + m[1][1] * vector.y + m[1][2] * vector.z + m[1][3] * vector.w,
            m[2][0] * vector.x + m[2][1] * vector.y + m[2][2] * vector.z + m[2][3] * vector.w,
            m[3][0] * vector.x + m[3][1] * vector.y + m[3][2] * vector.z + m[3][3] * vector.w
        );
    }

    mat &operator+=(const mat &other) {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] += other.m[i][j];
        return *this;
    }

    mat &operator-=(const mat &other) {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] -= other.m[i][j];
        return *this;
    }

    mat &operator*=(const mat &other) {
        *this = *this * other;
        return *this;
    }

    mat &operator*=(float scalar) {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] *= scalar;
        return *this;
    }

    bool operator==(const mat &other) const {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                if (m[i][j] != other.m[i][j])
                    return false;
        return true;
    }

    bool operator!=(const mat &other) const {
        return !(*this == other);
    }
};
