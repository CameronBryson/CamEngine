#pragma once
#include "Vectors.hpp"
#include <cmath>

#include <MathUtil.hpp>

class quat
{
  public:
    float w = 1, x = 0, y = 0, z = 0;

    quat() = default;
    quat(const float w, const float x, const float y, const float z) : w(w), x(x), y(y), z(z)
    {
    }

    quat operator+(const quat &other) const
    {
        return {w + other.w, x + other.x, y + other.y, z + other.z};
    }

    quat operator-(const quat &other) const
    {
        return {w - other.w, x - other.x, y - other.y, z - other.z};
    }

    quat operator*(const quat &other) const
    {
        return {w * other.w - x * other.x - y * other.y - z * other.z,
                w * other.x + x * other.w + y * other.z - z * other.y,
                w * other.y - x * other.z + y * other.w + z * other.x,
                w * other.z + x * other.y - y * other.x + z * other.w};
    }

    quat operator*(const float scalar) const
    {
        return {w * scalar, x * scalar, y * scalar, z * scalar};
    }

    quat &operator+=(const quat &other)
    {
        w += other.w;
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    quat &operator-=(const quat &other)
    {
        w -= other.w;
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    quat &operator*=(const quat &other)
    {
        *this = *this * other;
        return *this;
    }

    quat &operator*=(const float scalar)
    {
        w *= scalar;
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    [[nodiscard]] float norm() const
    {
        return std::sqrt(w * w + x * x + y * y + z * z);
    }

    [[nodiscard]] quat normalized() const
    {
        const float n = norm();
        return n > 0 ? quat(w / n, x / n, y / n, z / n) : quat();
    }

    [[nodiscard]] quat conjugate() const
    {
        return {w, -x, -y, -z};
    }

    [[nodiscard]] quat inverse() const
    {
        const float n = norm();
        return n > 0 ? conjugate() * (1.0f / (n * n)) : quat();
    }

    vec3 operator*(const vec3 &v) const
    {
        vec3 u(x, y, z);
        float s = w;
        return u * 2.0f * MathUtil::dot_product(u, v) + v * (s * s - MathUtil::dot_product(u, u)) + MathUtil::cross_product(u, v) * 2.0f * s;
    }
};