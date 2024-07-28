#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include <cmath>

class Quat {
public:
    float w = 1, x = 0, y = 0, z = 0;

    Quat() = default;
    Quat(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}

    Quat operator+(const Quat& other) const {
        return {w + other.w, x + other.x, y + other.y, z + other.z};
    }

    Quat operator-(const Quat& other) const {
        return {w - other.w, x - other.x, y - other.y, z - other.z};
    }

    Quat operator*(const Quat& other) const {
        return {
            w * other.w - x * other.x - y * other.y - z * other.z,
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w
        };
    }

    Quat operator*(float scalar) const {
        return {w * scalar, x * scalar, y * scalar, z * scalar};
    }

    Quat& operator+=(const Quat& other) {
        w += other.w; x += other.x; y += other.y; z += other.z;
        return *this;
    }

    Quat& operator-=(const Quat& other) {
        w -= other.w; x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }

    Quat& operator*=(const Quat& other) {
        *this = *this * other;
        return *this;
    }

    Quat& operator*=(float scalar) {
        w *= scalar; x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }

    float norm() const {
        return std::sqrt(w * w + x * x + y * y + z * z);
    }

    Quat normalized() const {
        float n = norm();
        return n > 0 ? Quat(w / n, x / n, y / n, z / n) : Quat();
    }

    Quat conjugate() const {
        return {w, -x, -y, -z};
    }

    Quat inverse() const {
        float n = norm();
        return n > 0 ? conjugate() * (1.0f / (n * n)) : Quat();
    }
};

#endif // QUATERNION_HPP